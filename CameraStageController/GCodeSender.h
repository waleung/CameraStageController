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
    void checkReady();

public:
    void sendCode(QString gcode);
    bool isReady();

private:
    CameraStageController &camerastagecontroller;
    Logger &logger;

    bool ready;

    QTimer *readyTimer = nullptr;
};

#endif // GCODESENDER_H
