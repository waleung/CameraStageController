#ifndef GCODESENDER_H
#define GCODESENDER_H

#include <QObject>
#include <QTimer>
#include "CameraStageController.h"
#include "Logger.h"

class GCodeSender: public QObject
{
    Q_OBJECT

public:
    GCodeSender(CameraStageController &camerastagecontroller_, Logger &logger_, QObject *parent = nullptr);

public slots:
    void checkReady(); //This will be called every 10ms after a GCode command has been sent.

public:
    void sendCode(QString gcode);
    bool isReady();

private:
    CameraStageController &camerastagecontroller;
    Logger &logger;

    bool ready;

    QTimer *readyTimer = nullptr; //Timer for checkReady SLOT. The timer will start after a GCode command and sent and stops when it receives a return message.
};

#endif // GCODESENDER_H
