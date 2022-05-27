#include "Scripter.h"

Scripter::Scripter(CameraStageController &camerastagecontroller_, Logger &logger_, GCodeSender &gcodesender_, QObject *parent)
    : QObject{parent}, camerastagecontroller(camerastagecontroller_), logger(logger_), gcodesender(gcodesender_)
{
    scriptTimer = new QTimer(this);

    connect(scriptTimer, SIGNAL(timeout()), this, SLOT(runScript()));

    scriptStarted = false;
}

Scripter::~Scripter()
{
}

void Scripter::loadScript(QString scriptFile)
{
    script.clear();
    QFile inputFile(scriptFile);
    inputFile.open(QIODevice::ReadOnly);
    if (inputFile.isOpen())
    {
        QTextStream stream(&inputFile);
        for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine())
        {
            if (!line.isEmpty())
                script.push_back(line.split(";").first());
        }
    }
    logger.log("Script loaded: " + scriptFile.split("/").back(), YELLOW);
}

void Scripter::loadScript(std::vector<QString> scriptVector)
{
    script.clear();
    script = scriptVector;
}

void Scripter::runScript()
{
    if (gcodesender.isReady())
    {
        if (script.at(scriptCounter) == "M240")
        {
            logger.log("Send: M240");
            camerastagecontroller.captureImage();
        }
        else
            gcodesender.sendCode(script.at(scriptCounter));
        scriptCounter++;
    }

    if (scriptCounter >= script.size())
        stopScript();

}

void Scripter::startScript()
{
    logger.log("---------Starting Script---------", YELLOW);
    scriptCounter = 0;
    scriptTimer->start(10);
    scriptStarted = true;
}

void Scripter::stopScript()
{
    if (scriptStarted)
    {
        logger.log("---------Stopping Script---------", YELLOW);
        scriptTimer->stop();
        scriptStarted = false;
        emit finished();
    }
}
