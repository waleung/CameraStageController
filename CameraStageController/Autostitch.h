#ifndef AUTOSTITCH_H
#define AUTOSTITCH_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <iostream>

#include "Scripter.h"
#include "CameraStageController.h"
#include "StitcherWorker.h"

class AutoStitch : public QObject
{
    Q_OBJECT
public:
    explicit AutoStitch(Scripter &scripter_, CameraStageController &camerastagecontroller_, StitcherWorker &stitcherWorker_, QThread &stitcherThread_, QObject *parent = nullptr);

public slots:
    void run();
    void runStitcher();
    void progressBar();

signals:
    void progress(int value); //Signal to progress the progressbar
    void setMax(int max); //Signal to set the maximum value for the progressbar

public:
    std::vector<QString> scriptCreater(double start_x, double start_y); //Create a X * Y GRID gcode script
    void startAuto();

    void addStartingCoords(QStringList list);
    void setProperties(int x_size, int y_size, double x_pitch, double y_pitch);

private:
    QString createSaveDir(QString orgImageFolder); // Creates the directory to save the GRID images. The folder name will be a integer digit. E.g 003
    //The folder will be saved to a subfolder set by in the camera settings.

private:
    Scripter &scripter;
    CameraStageController &camerastagecontroller;
    StitcherWorker &stitcherWorker;

    QThread &stitcherThread;

    QVector< QVector<QString> > vector_of_start_coords;
    std::vector< std::vector<QString> > vector_of_scripts;

    int gridX_size;
    int gridY_size;
    double gridX_pitch;
    double gridY_pitch;

    int script_count; //Counts the number of scripts done
    int stitcher_count; // Count the number of stitches done
    int progress_count; //Is equal to script_count + progress_count

    QStringList listImageFolder;
    QString imageFolder;
    QString outputFolder;
    QString saveFolder;
};

#endif // AUTOSTITCH_H
