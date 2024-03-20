#include "UiDownloader.h"

UiDownloader::UiDownloader(std::shared_ptr<download::AbstractDownloader> downloader, QObject* parent)
    : QObject(parent) 
    , m_realDownloader(std::move(downloader))
{
    setStatus(Ready);
}

UiDownloader::~UiDownloader()
{
}

void UiDownloader::setRealDownloader(std::shared_ptr<download::AbstractDownloader> realDownloader)
{
    m_realDownloader = realDownloader;
}

std::shared_ptr<download::AbstractDownloader> UiDownloader::realDownloader()
{
    return m_realDownloader;
}

void UiDownloader::setFileName(const std::string& filename)
{
    m_filename = filename;
}

const std::string& UiDownloader::filename() const
{
    return m_filename;
}

void UiDownloader::start()
{
    m_realDownloader->start();
    setStatus(Downloading);
}

void UiDownloader::stop()
{
    m_realDownloader->stop();
    setStatus(Wait);
}

void UiDownloader::pause()
{
    m_realDownloader->pause();
    setStatus(Wait);
}

void UiDownloader::resume()
{
    m_realDownloader->resume();
    setStatus(Downloading);
}

void UiDownloader::downloadStatus()
{
    m_realDownloader->downloadStatus();
    emit update(m_realDownloader->info(), QString::fromStdString(m_realDownloader->stage()));
    if (m_realDownloader->status() == Finished)
    {
        setStatus(Finished);
    }
}

void UiDownloader::finish()
{
    emit finished(QString());
}
