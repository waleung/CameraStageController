#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSignalMapper>
#include <QList>
#include <QPainter>
#include <QThread>
#include <QDebug>
#include <QSettings>
#include <QCloseEvent>

#include "CameraStageController.h"
#include "dialogs/camerasettingsdialog.h"
#include "Scripter.h"
#include "Logger.h"
#include "GCodeSender.h"
#include "Stitcher.h"
#include "StitcherWorker.h"
#include "Autostitch.h"
#include "dialogs/helpdialog.h"
#include "dialogs/stitcherhelpdialog.h"
#include "dialogs/autohelpdialog.h"

#define MOVE_Y_POSITIVE 0
#define MOVE_Y_NEGATIVE 1
#define MOVE_X_POSITIVE 2
#define MOVE_X_NEGATIVE 3

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    //Camera buttons and functions
    void displayImage();
    void connectCamera();
    void disconnectCamera();
    void captureImage();

    //Opens camerasettingsdialog
    void openCameraSettings();

    //Motor controller buttons and functions
    void sendGCodeCommand();

    void getCOMports();
    void connectCOMports();

    void moveStage(int direction);
    void setRelativeMode();
    void stopStage();
    void homing();

    //Script buttons
    void loadScript();
    void startScript();
    void stopScript();

    void writeLog(QString message, int color); //Writes the message to the mainwindow console

    //Stitcher functions and buttons
    void openImageFolder();
    void openOutputFolder();
    void startStitcher();

    //Button controls for adding/moving/removing starting coordinates from the list
    void addCoordinates();
    void removeCoordinates();
    void moveCoordUp();
    void moveCoordDown();

    //Auto stitcher functions and buttons
    void startAuto();
    void setMaxProgressBar(int max);
    void setProgress(int value);

    //Help buttons
    void showHelp();
    void showStitcherHelp();
    void showAutoHelp();

    void quitApp(); //Quits the application

public:
    void setLiveImageBlack();
    void readSettings(); //Read settings from the config.ini file
    void writeSettings(); //Save settings to the config.ini file

    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    QTimer *liveViewTimer = nullptr;

    QSignalMapper mapper;

    int lineNumber = 0;

    CameraStageController camerastagecontroller;
    CameraSettingsDialog *camerasettingsdialog = nullptr;
    Scripter *scripter = nullptr;
    Logger *logger = nullptr;
    GCodeSender *gcodesender = nullptr;

    Stitcher stitcher;
    StitcherWorker *stitcherWorker = nullptr; //The stitcher is working on an another thread (stitcherThread). This is done so the main mindow is not blocked while stitching
    QThread *stitcherThread = nullptr;

    AutoStitch *autoStitch = nullptr;

    HelpDialog *helpdialog = nullptr;
    StitcherHelpDialog *stitcherhelpdialog = nullptr;

    AutoHelpDialog *autohelpdialog = nullptr;

    QSettings *autoSettings = nullptr;

};
#endif // MAINWINDOW_H
