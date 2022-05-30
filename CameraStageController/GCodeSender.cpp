#include "GCodeSender.h"

GCodeSender::GCodeSender(CameraStageController &camerastagecontroller_, Logger &logger_, QObject *parent)
    : QObject{parent}, camerastagecontroller(camerastagecontroller_), logger(logger_)
{
    readyTimer = new QTimer(this);
    ready = true;
    connect(readyTimer, SIGNAL(timeout()), this, SLOT(checkReady()));
}

void GCodeSender::sendCode(QString gcode)
{
    if (camerastagecontroller.isSerialConnected())
    {
        camerastagecontroller.sendGCode(gcode);
        logger.log("Send: " + gcode);
        readyTimer->start(10); //Starts the timer to checkReady() every 10ms
        ready = false;
    }
    else
        logger.log("Serial is not connected!", RED);
}

void GCodeSender::checkReady()
{
    if (camerastagecontroller.stageMessageRec())
    {
        readyTimer->stop(); //Stops the timer
//        if (camerastagecontroller.getStageMessage().trimmed() == "GCode not recognized")
//            logger.log("Return: " + camerastagecontroller.getStageMessage().trimmed(), RED);
//        else
        logger.log("Return: " + camerastagecontroller.getStageMessage().trimmed());
        ready = true;
    }
}

bool GCodeSender::isReady()
{
    return ready;
}
