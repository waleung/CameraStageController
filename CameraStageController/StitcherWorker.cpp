#include "StitcherWorker.h"

StitcherWorker::StitcherWorker(Stitcher &stitcher_, std::ostream &stream_, QObject *parent)
    : QObject{parent}, stitcher(stitcher_), stream(stream_)
{
    old_buf = stream.rdbuf();
    stream.rdbuf(this);

    stitcher.GRID_X = 9;
    stitcher.GRID_Y = 9;
}

StitcherWorker::~StitcherWorker()
{
    stream.rdbuf(old_buf);
}

void StitcherWorker::process()
{
    stitcher.clearLoadedImages();
    stitcher.loadImages(loadFolder.toStdString().c_str());
    if (stitcher.isReady())
    {
        if (stitcher.stitchImages(maskEnable))
            stitcher.saveImage(outputFolder.toStdString().c_str());
    }
    emit finished();
}

void StitcherWorker::setLoadImageFolder(QString folder)
{
    loadFolder = folder;
}

void StitcherWorker::setOutputFolder(QString folder)
{
    outputFolder = folder;
}

void StitcherWorker::setMaskEnable(bool enable)
{
    maskEnable = enable;
}

StitcherWorker::int_type StitcherWorker::overflow(int_type v)
{
    if (v == '\n')
    {
        emit logToConsole(QString::fromStdString(m_string));;
        m_string.clear();
    }
    else
        m_string.push_back(v);

    return v;
}

std::streamsize  StitcherWorker::xsputn(const char *p, std::streamsize n)
{
    m_string.append(p, p + n);

    int pos = 0;
    while (pos != std::string::npos)
    {
        pos = m_string.find('\n');
        if (pos != std::string::npos)
        {
            std::string tmp(m_string.begin(), m_string.begin() + pos);
            emit logToConsole(QString::fromStdString(tmp));
            m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
        }
    }

    return n;
}
