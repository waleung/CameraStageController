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
    void progress(int value);
    void setMax(int max);

public:
    std::vector<QString> scriptCreater(double start_x, double start_y);
    void startAuto();

    void addStartingCoords(QStringList list);
    void setProperties(int x_size, int y_size, double x_pitch, double y_pitch);

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

    int script_count;
    int stitcher_count;
    int progress_count;

    QStringList listImageFolder;
    QString imageFolder;
    QString outputFolder;
};

#endif // AUTOSTITCH_H
