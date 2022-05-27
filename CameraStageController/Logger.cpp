#include "Logger.h"

Logger::Logger(QObject *parent)
    : QObject{parent}
{

}

void Logger::log(QString message, int color)
{
    QString sendMessage;

    sendMessage = QString::number(lineCount) + " -> " + message;
    lineCount++;

    emit logMessage(sendMessage, color);
}
