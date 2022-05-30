#ifndef STITCHERWORKER_H
#define STITCHERWORKER_H

#include <QObject>

#include "Stitcher.h"

//Note: When this class is initilized, all std::cout will be redirected to the main window console output. When deleted it will redirect output back.
//This class is for the use of a QThread. moveToThread(QThread);

class StitcherWorker : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT
public:
    StitcherWorker(Stitcher &stitcher_, std::ostream &stream_, QObject *parent = nullptr);
    ~StitcherWorker();

public:
    void setOutputFolder(QString folder);
    void setLoadImageFolder(QString folder);
    void setMaskEnable(bool enable);

public slots:
    void process();

signals:
    void finished();
    //Signal for ouputing the redirected std::cout ouput.
    void logToConsole(QString message);

private:
    Stitcher &stitcher;
    std::ostream &stream;
    std::streambuf *old_buf;
    std::string m_string;

    QString outputFolder;
    QString loadFolder;

    bool maskEnable;

//overloaded functions to redirect std::cout to GUI console.
protected:
    virtual std::streamsize xsputn(const char *p, std::streamsize n);
    virtual int_type overflow(int_type v);
};

#endif // STITCHERWORKER_H
