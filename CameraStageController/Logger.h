#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

enum Color {
    RED = 1,
    YELLOW = 2,
    WHITE = 3
};

class Logger: public QObject
{
    Q_OBJECT
public:
    Logger(QObject *parent = nullptr);

public slots:
    void log(QString message, int color = WHITE);

signals:
    void logMessage(QString message, int color);

private:
    int lineCount = 0;
};

#endif // LOGGER_H
