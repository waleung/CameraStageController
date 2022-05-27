#ifndef STITCHERWORKER_H
#define STITCHERWORKER_H

#include <QObject>

#include "Stitcher.h"

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
    void logToConsole(QString message);
    void next();

private:
    Stitcher &stitcher;
    std::ostream &stream;
    std::streambuf *old_buf;
    std::string m_string;

    QString outputFolder;
    QString loadFolder;

    bool maskEnable;
protected:
    virtual std::streamsize xsputn(const char *p, std::streamsize n);
    virtual int_type overflow(int_type v);
};

#endif // STITCHERWORKER_H
