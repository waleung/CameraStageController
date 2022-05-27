#include "camerasettingsdialog.h"
#include "ui_camerasettingsdialog.h"

CameraSettingsDialog::CameraSettingsDialog(CameraStageController &camerastagecontroller_, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraSettingsDialog),
    camerastagecontroller(camerastagecontroller_)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(setCameraSettings()));
    connect(ui->BrowsePushButton, SIGNAL(clicked()), this, SLOT(browseDir()));
}

//---------------Slots------------------------
void CameraSettingsDialog::setCameraSettings()
{
    //Module Name
    QString module_name = ui->nameLineEdit->text();

    //Save Directory
    QString temp_dir = getSaveDirectoryText();

    if (!module_name.isEmpty())
    {
        //qDebug() << temp_dir.split("/").rbegin()[0];
        if (!(temp_dir.split("/").rbegin()[0] == module_name + "_" + QDate::currentDate().toString("yyyy_MM_dd")))
        {
            //QString dir_path = QCoreApplication::applicationDirPath() + "/Pictures/" + module_name + "_" + QDate::currentDate().toString("yyyy_MM_dd");
            QString dir_path = temp_dir + "/" + module_name + "_" + QDate::currentDate().toString("yyyy_MM_dd");

            QDir dir(dir_path);
            if (!dir.exists())
            {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Directory Name", "Save pictures to \" " + dir_path + " \" ?", QMessageBox::Yes|QMessageBox::No);
                if (reply == QMessageBox::Yes)
                {
                  temp_dir = dir_path;
                  dir.mkpath(temp_dir);
                }
                else
                    temp_dir = camerastagecontroller.settings.saveDirectory;
            }
        }
    }
//    else
//        temp_dir = camerastagecontroller.settings.saveDirectory;


    QDir tempDir(temp_dir);
    if (!tempDir.exists())
        tempDir.mkpath(temp_dir);

    setSaveDirectoryText(temp_dir);
    camerastagecontroller.setSaveDirectory(temp_dir);

    //ISO speed
    int32_t isoHex = ui->ISOspeedComboBox->itemData(ui->ISOspeedComboBox->currentIndex()).toInt();
    camerastagecontroller.setISOspeed(isoHex);

    //Shutter speed
    int32_t shutterHex = ui->shutterSpeedComboBox->itemData(ui->shutterSpeedComboBox->currentIndex()).toInt();
    camerastagecontroller.setShutterSpeed(shutterHex);
}

void CameraSettingsDialog::browseDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), camerastagecontroller.settings.saveDirectory, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        setSaveDirectoryText(dir);
    //camerastagecontroller.setSaveDirectory(dir);
}
//---------------Slots------------------------

CameraSettingsDialog::~CameraSettingsDialog()
{
    delete ui;
}

void CameraSettingsDialog::setSaveDirectoryText(QString directory)
{
    ui->saveDirectory->setText(directory);
}

QString CameraSettingsDialog::getSaveDirectoryText()
{
    return ui->saveDirectory->text();
}

QString CameraSettingsDialog::getISOspeedText()
{
    return ui->ISOspeedComboBox->currentText();
}

void CameraSettingsDialog::setISOspeedText(int32_t ISOspeed)
{
    int index = ui->ISOspeedComboBox->findData(ISOspeed);
    if (index != -1)
       ui->ISOspeedComboBox->setCurrentIndex(index);
}

void CameraSettingsDialog::setAvailableISO()
{
    for (int i = 0; i < camerastagecontroller.settings.arraySizeISO; i++)
        ui->ISOspeedComboBox->addItem(ISOspeedMap[camerastagecontroller.settings.listISOspeed[i]], camerastagecontroller.settings.listISOspeed[i]);
}

void CameraSettingsDialog::setShutterSpeedText(int32_t shutterSpeed)
{
    int index = ui->shutterSpeedComboBox->findData(shutterSpeed);
    if (index != -1)
       ui->shutterSpeedComboBox->setCurrentIndex(index);
}

void CameraSettingsDialog::setAvailableShutter()
{
    for (int i = 0; i < camerastagecontroller.settings.arraySizeShutter; i++)
        ui->shutterSpeedComboBox->addItem(ShutterSpeedMap[camerastagecontroller.settings.listShutterSpeed[i]], camerastagecontroller.settings.listShutterSpeed[i]);
}

void CameraSettingsDialog::clear()
{
    ui->ISOspeedComboBox->clear();
    ui->shutterSpeedComboBox->clear();
    ui->saveDirectory->clear();
}
