#include "UiDownloader.h"

UiDownloader::UiDownloader(std::shared_ptr<download::AbstractDownloader> downloader, QObject* parent)
    : QObject(parent) 
    , m_realDownloader(std::move(downloader))
{
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

void UiDownloader::start()
{
    m_realDownloader->start();
}

void UiDownloader::stop()
{
    m_realDownloader->stop();
}

void UiDownloader::pause()
{
    m_realDownloader->pause();
}

void UiDownloader::resume()
{
    m_realDownloader->resume();
}

void UiDownloader::downloadStatus()
{
    m_realDownloader->downloadStatus();
}

void UiDownloader::finish()
{
    emit finished(QString());
}
