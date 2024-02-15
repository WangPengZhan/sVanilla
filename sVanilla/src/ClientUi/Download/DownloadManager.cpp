#include "DownloadManager.h"

DownloadManager::DownloadManager(QObject* parent)
    : QObject(parent)
{
}

void DownloadManager::addItem(std::shared_ptr<download::AbstractDownloader> downloader)
{
    m_thread.addTaks(downloader);
}
