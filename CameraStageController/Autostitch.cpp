#include "Autostitch.h"

AutoStitch::AutoStitch(Scripter &scripter_, CameraStageController &camerastagecontroller_, StitcherWorker &stitcherWorker_, QThread &stitcherThread_, QObject *parent)
    : QObject{parent}, scripter(scripter_), camerastagecontroller(camerastagecontroller_), stitcherWorker(stitcherWorker_), stitcherThread(stitcherThread_)
{
    script_count = 0;
    stitcher_count = 0;
    progress_count = -1;

    autoEnabled = false;
}

std::vector<QString> AutoStitch::scriptCreater(double start_x, double start_y)
{
    std::vector<QString> script;

    script.push_back("G90");
    script.push_back("G1 X" + QString::number(start_x) + " Y" + QString::number(start_y));
    script.push_back("G91");

    for (int i = 0; i < gridY_size; ++i)
    {
        for (int j = 0; j < gridX_size; ++j)
        {
            script.push_back("G4 P1000");
            script.push_back("M240");
            script.push_back("G4 P1000");

            if (j < (gridX_size - 1))
            {
                if ((i % 2) == 0)
                    script.push_back("G1 X" + QString::number(gridX_pitch));
                else
                    script.push_back("G1 X-" + QString::number(gridX_pitch));
            }
        }

        if (i < (gridY_size - 1))
            script.push_back("G1 Y-" + QString::number(gridY_pitch));
    }

    script.push_back("G4 P3000"); //Wait for the computer to download the captured images
    script.push_back("M114");

    return script;
}

void AutoStitch::setProperties(int x_size, int y_size, double x_pitch, double y_pitch)
{
    gridX_size = x_size;
    gridY_size = y_size;
    gridX_pitch = x_pitch;
    gridY_pitch = y_pitch;
}

void AutoStitch::addStartingCoords(QStringList list)
{
    vector_of_start_coords.clear();

    for (int i = 0; i < list.count(); ++i)
        vector_of_start_coords.push_back(list.at(i).split(",").toVector());
}

void AutoStitch::startAuto()
{
    connect(&scripter, SIGNAL(finished()), this, SLOT(progressBar()));
    connect(&stitcherWorker, SIGNAL(finished()), this, SLOT(progressBar()));

    autoEnabled = true;

    vector_of_scripts.clear();
    listImageFolder.clear();

    for (int i = 0; i < vector_of_start_coords.count(); ++i)
        vector_of_scripts.push_back(scriptCreater(vector_of_start_coords.at(i).at(0).toDouble(), vector_of_start_coords.at(i).at(1).toDouble()));

    emit setMax((vector_of_scripts.size() * 2) + 1); //For progressbar

    script_count = 0;
    stitcher_count = 0;
    progress_count = 0;

    imageFolder = camerastagecontroller.settings.saveDirectory; 
    saveFolder = createSaveDir(imageFolder);
    outputFolder = saveFolder + "/Stitched/";

    connect(&scripter, SIGNAL(finished()), this, SLOT(run()));

    emit autoState(true);

    run();
}

void AutoStitch::run()
{
    if ((script_count < vector_of_scripts.size()) && autoEnabled)
    {
        camerastagecontroller.setSaveDirectory(saveFolder + "/GRID_" + QString::number(script_count));
        listImageFolder.append(camerastagecontroller.settings.saveDirectory);

        scripter.loadScript(vector_of_scripts.at(script_count));
        scripter.startScript();
        ++script_count;

        //emit progress(++progress_count);
    }
    else if (autoEnabled)
    {
        disconnect(&scripter, SIGNAL(finished()), this, SLOT(run()));

        connect(&stitcherWorker, SIGNAL(finished()), this, SLOT(runStitcher()));
        runStitcher();
    }
}

void AutoStitch::runStitcher()
{
    if ((stitcher_count < listImageFolder.count()) && autoEnabled)
    {
        if (stitcherThread.wait(1000))
        {
            stitcherWorker.setLoadImageFolder(listImageFolder.at(stitcher_count));

            QDir dir(outputFolder);
            if (!dir.exists())
                dir.mkpath(outputFolder);

            stitcherWorker.setOutputFolder(outputFolder + listImageFolder.at(stitcher_count).split("/").rbegin()[0] + ".JPG");
            stitcherWorker.setMaskEnable(false);
            //stitcherWorker.process();
            stitcherThread.start();
            ++stitcher_count;

            //emit progress(++progress_count);
        }
    }
    else if (autoEnabled)
    {
        camerastagecontroller.setSaveDirectory(imageFolder);
        disconnect(&stitcherWorker, SIGNAL(finished()), this, SLOT(runStitcher()));

        stitcherWorker.setLoadImageFolder(outputFolder);
        stitcherWorker.setOutputFolder(outputFolder + imageFolder.split("/").rbegin()[0] + "_Full.JPG");
        stitcherWorker.setMaskEnable(true);
        //stitcherWorker.process();
        stitcherThread.start();
        ++stitcher_count;

        //emit progress(++progress_count);
        emit autoState(false);
        autoEnabled = false;
    }
}

void AutoStitch::progressBar()
{
    emit progress(++progress_count);

    if (script_count >= vector_of_scripts.size())
        disconnect(&scripter, SIGNAL(finished()), this, SLOT(progressBar()));
    if (stitcher_count > listImageFolder.count())
        disconnect(&stitcherWorker, SIGNAL(finished()), this, SLOT(progressBar()));
}

QString AutoStitch::createSaveDir(QString orgImageFolder)
{
    for (int i = 1; i <= 100; ++i)
    {
        QString dirname = orgImageFolder + "/" + QString::number(i).rightJustified(3, '0');

        QDir dir(dirname);
        if (!dir.exists())
            return dirname;
    }
    return orgImageFolder + "/" + QString::number(0).rightJustified(3, '0');
}

void AutoStitch::stopAuto()
{
    disconnect(&stitcherWorker, SIGNAL(finished()), this, SLOT(runStitcher()));
    disconnect(&scripter, SIGNAL(finished()), this, SLOT(run()));

    autoEnabled = false;
    scripter.stopScript();

    stitcherThread.quit();
    stitcherThread.wait();

    camerastagecontroller.setSaveDirectory(imageFolder);

    emit autoState(false);
}

bool AutoStitch::isEnabled()
{
    return autoEnabled;
}
