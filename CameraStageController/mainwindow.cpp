#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    liveViewTimer = new QTimer(this);

    logger = new Logger;
    gcodesender = new GCodeSender(camerastagecontroller, *logger);
    scripter = new Scripter(camerastagecontroller, *logger, *gcodesender);

    camerasettingsdialog = new CameraSettingsDialog(camerastagecontroller);
    helpdialog = new HelpDialog();
    stitcherhelpdialog = new StitcherHelpDialog();
    autohelpdialog = new AutoHelpDialog();

    stitcherWorker = new StitcherWorker(stitcher, std::cout);

    stitcherThread = new QThread(this);
    stitcherWorker->moveToThread(stitcherThread);

    autoStitch = new AutoStitch(*scripter, camerastagecontroller, *stitcherWorker, *stitcherThread);

    autoSettings = new QSettings(QString(QCoreApplication::applicationDirPath() + "/config.ini"), QSettings::IniFormat);

    //Timers
    connect(liveViewTimer, SIGNAL(timeout()), this, SLOT(displayImage()));

    //Camera signals
    connect(ui->actionConnect_Camera, SIGNAL(triggered()), this, SLOT(connectCamera()));
    connect(ui->actionDisconnect_Camera, SIGNAL(triggered()), this, SLOT(disconnectCamera()));
    connect(ui->captureButton, SIGNAL(clicked()), this, SLOT(captureImage()));
    connect(ui->actionSettings_Camera, SIGNAL(triggered()), this, SLOT(openCameraSettings()));

    //GCode signals
    connect(ui->gCodelineEdit, SIGNAL(returnPressed()), this, SLOT(sendGCodeCommand()));
    connect(ui->SendpushButton, SIGNAL(clicked()), this, SLOT(sendGCodeCommand()));

    //Com Ports
    connect(ui->menuStage, SIGNAL(aboutToShow()), this, SLOT(getCOMports()));

    //On Exit
    //connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quitApp()));

    //Move buttons
    connect(ui->upButton, SIGNAL(clicked()), &mapper, SLOT(map()));
    mapper.setMapping(ui->upButton, MOVE_Y_POSITIVE);

    connect(ui->downButton, SIGNAL(clicked()), &mapper, SLOT(map()));
    mapper.setMapping(ui->downButton, MOVE_Y_NEGATIVE);

    connect(ui->leftButton, SIGNAL(clicked()), &mapper, SLOT(map()));
    mapper.setMapping(ui->leftButton, MOVE_X_NEGATIVE);

    connect(ui->rightButton, SIGNAL(clicked()), &mapper, SLOT(map()));
    mapper.setMapping(ui->rightButton, MOVE_X_POSITIVE);

    connect(&mapper, SIGNAL(mapped(int)), this, SLOT(moveStage(int)));

    connect(ui->RelativeModePushButton, SIGNAL(clicked()), this, SLOT(setRelativeMode()));

    connect(ui->StopPushButton, SIGNAL(clicked()), this, SLOT(stopStage()));

    connect(ui->HomePushButton, SIGNAL(clicked()), this, SLOT(homing()));

    //Scripter
    connect(ui->LoadScriptPushButton, SIGNAL(clicked()), this, SLOT(loadScript()));
    connect(ui->StartScriptPushButton, SIGNAL(clicked()), this, SLOT(startScript()));
    connect(ui->StopScriptPushButton, SIGNAL(clicked()), this, SLOT(stopScript()));

    //Logging
    connect(logger, SIGNAL(logMessage(QString,int)), this, SLOT(writeLog(QString,int)));

    //Stitcher
    connect(ui->ImageFolderPushButton, SIGNAL(clicked()), this, SLOT(openImageFolder()));
    connect(ui->OutputFolderPushButton, SIGNAL(clicked()), this, SLOT(openOutputFolder()));
    connect(ui->StartStitcherPushButton, SIGNAL(clicked()), this, SLOT(startStitcher()));

    connect(stitcherThread, SIGNAL(started()), stitcherWorker, SLOT(process()));
    connect(stitcherWorker, SIGNAL(finished()), stitcherThread, SLOT(quit()));

    //connect(stitcherWorker, SIGNAL(finished()), stitcherWorker, SLOT(deleteLater()));
    //connect(stitcherThread, SIGNAL(finished()), stitcherThread, SLOT(deleteLater()));

    connect(stitcherWorker, SIGNAL(logToConsole(QString)), logger, SLOT(log(QString)));

    //Automation
    connect(ui->AddCoordPushButton, SIGNAL(clicked()), this, SLOT(addCoordinates()));
    connect(ui->RemovePushButton, SIGNAL(clicked()), this, SLOT(removeCoordinates()));
    connect(ui->StartPushButton, SIGNAL(clicked()), this, SLOT(startAuto()));
    connect(ui->autoUpPushButton, SIGNAL(clicked()), this, SLOT(moveCoordUp()));
    connect(ui->autoDownPushButton, SIGNAL(clicked()), this, SLOT(moveCoordDown()));

    connect(autoStitch, SIGNAL(setMax(int)), this, SLOT(setMaxProgressBar(int)));
    connect(autoStitch, SIGNAL(progress(int)), this, SLOT(setProgress(int)));

    connect(autoStitch, SIGNAL(autoState(bool)), this, SLOT(autoChangeButton(bool)));

    //Help buttons
    connect(ui->HelpPushButton, SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(ui->HelpStitcherPushButton, SIGNAL(clicked()), this, SLOT(showStitcherHelp()));
    connect(ui->AutoStitcherHelpPushButton, SIGNAL(clicked()), this, SLOT(showAutoHelp()));

    setLiveImageBlack();

    //Startup
    readSettings();
    connectCamera();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setLiveImageBlack()
{
    QPixmap pix(800, 600);
    pix.fill(Qt::black);

    ui->liveView->setPixmap(pix);
}

void MainWindow::displayImage()
{
    ui->liveView->setPixmap(QPixmap::fromImage(camerastagecontroller.getCameraImage()));
}

void MainWindow::connectCamera()
{
    if (!camerastagecontroller.isCameraConnected())
    {
        if (camerastagecontroller.connectCamera())
        {
            liveViewTimer->start(20);
            ui->actionDisconnect_Camera->setEnabled(true);
            ui->actionSettings_Camera->setEnabled(true);

            camerasettingsdialog->setAvailableISO();
            camerasettingsdialog->setAvailableShutter();

            QString cameraInfo = "Connected to: " + QString::fromLocal8Bit(camerastagecontroller.settings.device);

            logger->log(cameraInfo);
        }
        else
            logger->log("Could not find a camera to connect", RED);
    }
    else
        logger->log("Camera is already connected!", RED);
}

void MainWindow::disconnectCamera()
{
    camerastagecontroller.disconnectCamera();
    liveViewTimer->stop();
    ui->actionDisconnect_Camera->setEnabled(false);
    ui->actionSettings_Camera->setEnabled(false);

    camerasettingsdialog->clear();
    setLiveImageBlack();

    QString cameraInfo = "Disconnected to: " + QString::fromLocal8Bit(camerastagecontroller.settings.device);
    logger->log(cameraInfo);
}

void MainWindow::captureImage()
{
    camerastagecontroller.captureImage();
}

void MainWindow::openCameraSettings()
{
    camerasettingsdialog->setSaveDirectoryText(camerastagecontroller.settings.saveDirectory);
    camerasettingsdialog->setISOspeedText(camerastagecontroller.settings.ISOspeed);
    camerasettingsdialog->setShutterSpeedText(camerastagecontroller.settings.shutterSpeed);

    camerasettingsdialog->exec();
}

void MainWindow::sendGCodeCommand()
{
    QString gcode = ui->gCodelineEdit->text();
    gcodesender->sendCode(gcode);
    ui->gCodelineEdit->clear();
}

void MainWindow::connectCOMports()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString com = action->text().split(" ").at(0);

    if (camerastagecontroller.connectToCOM(com))
        logger->log("Connected to Port: " + com);
    else
        logger->log("Could not connect to Port: " + com, RED);
}

void MainWindow::getCOMports()
{
    QList list = QSerialPortInfo::availablePorts();

    ui->menuStage->clear();

    QMenu *connectMenu = ui->menuStage->addMenu("Connect");
    for (int i = 0; i < list.count(); i++)
    {
        QAction *actions = connectMenu->addAction(list.at(i).portName() + " " + list.at(i).description());
        connect(actions, SIGNAL(triggered()), this, SLOT(connectCOMports()));
    }
}

void MainWindow::moveStage(int direction)
{
    if (gcodesender->isReady())
    {
        QString move = QString::number(ui->MoveDoubleSpinBox->value());
        switch (direction)
        {
            //Axis directions are defined in mainwindow.h
            case MOVE_Y_POSITIVE:
                gcodesender->sendCode("G0 Y" + move);
                break;

            case MOVE_Y_NEGATIVE:
                gcodesender->sendCode("G0 Y-" + move);
                break;

            case MOVE_X_POSITIVE:
                gcodesender->sendCode("G0 X" + move);
                break;

            case MOVE_X_NEGATIVE:
                gcodesender->sendCode("G0 X-" + move);
                break;
        }
    }
    else
        logger->log("Busy", YELLOW);
}

void MainWindow::setRelativeMode()
{
    gcodesender->sendCode("G91");
}

void MainWindow::stopStage()
{
    gcodesender->sendCode("M112");
    scripter->stopScript();
}

void MainWindow::homing()
{
    gcodesender->sendCode("G28");
}

void MainWindow::loadScript()
{
    QString scriptFile = QFileDialog::getOpenFileName(this, tr("Open GCode File"), QCoreApplication::applicationDirPath(), tr("GCODE (*.GCODE *.gcode)"));
    if (!scriptFile.isEmpty())
        scripter->loadScript(scriptFile);
}

void MainWindow::startScript()
{
    scripter->startScript();
}

void MainWindow::stopScript()
{
    scripter->stopScript();
}

void MainWindow::writeLog(QString message, int color)
{
    switch (color)
    {
        case WHITE:
            ui->commandLine->setTextBackgroundColor(QColor(255, 255, 255));
            break;

        case RED:
            ui->commandLine->setTextBackgroundColor(QColor(255, 0, 0));
            break;

        case YELLOW:
            ui->commandLine->setTextBackgroundColor(QColor(255, 255, 0));
            break;
    }

    ui->commandLine->append(message);
}

void MainWindow::openImageFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui->ImageFolderLineEdit->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        ui->ImageFolderLineEdit->setText(dir);
}

void MainWindow::openOutputFolder()
{
    //QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),  ui->ImageFolderLineEdit->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QString dir = QFileDialog::getSaveFileName(this, tr("Save File"), QCoreApplication::applicationDirPath(), tr("JPG (*.JPG *.jpg)"));
    if (!dir.isEmpty())
        ui->OutputFolderLineEdit->setText(dir);
}

void MainWindow::startStitcher()
{
    stitcherWorker->setLoadImageFolder(ui->ImageFolderLineEdit->text());
    stitcherWorker->setOutputFolder(ui->OutputFolderLineEdit->text());
    stitcherWorker->setMaskEnable(ui->MaskEnableCheckBox->isChecked());
    stitcherThread->start();
}

void MainWindow::addCoordinates()
{
    ui->CoordListWidget->addItem(QString::number(ui->XCoordDoubleSpinBox->value()) + ", " + QString::number(ui->YCoordDoubleSpinBox->value()));
}

void MainWindow::moveCoordUp()
{
    int currentRow = ui->CoordListWidget->currentRow();

    if (currentRow > 0)
    {
        QListWidgetItem* currentItem = ui->CoordListWidget->takeItem(currentRow);
        ui->CoordListWidget->insertItem(currentRow - 1, currentItem->text());
        ui->CoordListWidget->setCurrentRow(currentRow - 1);
    }
}

void MainWindow::moveCoordDown()
{
    int currentRow = ui->CoordListWidget->currentRow();

    if ((currentRow < (ui->CoordListWidget->count() - 1)) && (currentRow >= 0))
    {
        QListWidgetItem* currentItem = ui->CoordListWidget->takeItem(currentRow);
        ui->CoordListWidget->insertItem(currentRow + 1, currentItem->text());
        ui->CoordListWidget->setCurrentRow(currentRow + 1);
    }
}

void MainWindow::removeCoordinates()
{
    qDeleteAll(ui->CoordListWidget->selectedItems());
}

void MainWindow::startAuto()
{
    if (!autoStitch->isEnabled())
    {
        QStringList listCoords;

        autoStitch->setProperties(ui->GridXSizeDoubleSpinBox->value(), ui->GridYSizeDoubleSpinBox->value(),
                                  ui->GridXpitchDoubleSpinBox->value(), ui->GridYpitchDoubleSpinBox->value());

        for (int i = 0; i < ui->CoordListWidget->count(); ++i)
            listCoords.append(ui->CoordListWidget->item(i)->text());

        autoStitch->addStartingCoords(listCoords);
        autoStitch->startAuto();
    }
    else
        autoStitch->stopAuto();
}

void MainWindow::setMaxProgressBar(int max)
{
    ui->progressBar->reset();
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(max);
}

void MainWindow::setProgress(int value)
{
    ui->progressBar->setValue(value);
}

void MainWindow::autoChangeButton(bool enabled)
{
    if (enabled)
        ui->StartPushButton->setText("Stop!");
    else
        ui->StartPushButton->setText("Start!");
}

void MainWindow::showHelp()
{
    helpdialog->exec();
}

void MainWindow::showStitcherHelp()
{
    stitcherhelpdialog->exec();
}

void MainWindow::showAutoHelp()
{
    autohelpdialog->exec();
}

void MainWindow::readSettings()
{
    ui->GridXSizeDoubleSpinBox->setValue(autoSettings->value("GridX_Size").value<double>());
    ui->GridYSizeDoubleSpinBox->setValue(autoSettings->value("GridY_Size").value<double>());

    ui->GridXpitchDoubleSpinBox->setValue(autoSettings->value("GridX_Pitch").value<double>());
    ui->GridYpitchDoubleSpinBox->setValue(autoSettings->value("GridY_Pitch").value<double>());

    ui->CoordListWidget->addItems(autoSettings->value("Starting_Coords").value<QStringList>());
}

void MainWindow::writeSettings()
{
    QStringList listCoords;

    autoSettings->setValue("GridX_Size", ui->GridXSizeDoubleSpinBox->value());
    autoSettings->setValue("GridY_Size", ui->GridYSizeDoubleSpinBox->value());

    autoSettings->setValue("GridX_Pitch", ui->GridXpitchDoubleSpinBox->value());
    autoSettings->setValue("GridY_Pitch", ui->GridYpitchDoubleSpinBox->value());

    for (int i = 0; i < ui->CoordListWidget->count(); ++i)
        listCoords.append(ui->CoordListWidget->item(i)->text());

    autoSettings->setValue("Starting_Coords", listCoords);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Quit CameraStageController?",
                                                                    tr("Are you sure?\n"),
                                                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        writeSettings();
        quitApp();
        event->accept();
    }

    else
    {
        event->ignore();
    }
}

void MainWindow::quitApp()
{
    camerastagecontroller.disconnectCamera();
    //camerastagecontroller.disconnectCOM();
    camerasettingsdialog->clear();
    stitcherThread->quit();
    if(!stitcherThread->wait(5000))
    {
        stitcherThread->terminate();
        stitcherThread->wait();
    }
}
