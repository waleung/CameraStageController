#ifndef SCRIPTER_H
#define SCRIPTER_H

#include <QString>
#include <vector>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QTimer>

#include "CameraStageController.h"
#include "Logger.h"
#include "GCodeSender.h"

class Scripter : public QObject
{
    Q_OBJECT

public:
    Scripter(CameraStageController &camerastagecontroller_, Logger &logger_, GCodeSender &gcodesender_, QObject *parent = nullptr);
    ~Scripter();

public slots:
    void runScript();

signals:
    void finished();

public:
    void loadScript(QString scriptFile);
    void loadScript(std::vector<QString> scriptVector);
    void startScript();
    void stopScript();

private:
    std::vector<QString> script;
    QTimer *scriptTimer = nullptr;

    int scriptCounter = 0;

    bool scriptStarted;

    CameraStageController &camerastagecontroller;
    Logger &logger;
    GCodeSender &gcodesender;
};

#endif // SCRIPTER_H
