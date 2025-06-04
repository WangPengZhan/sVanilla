#include "DownloadManager.h"

#include <memory>

#include "MainWindow/SApplication.h"
#include "Download/DownloadStatusThread.h"

DownloadManager::DownloadManager(QObject* parent)
    : QObject(parent)
{
}

void DownloadManager::addItem(std::shared_ptr<download::AbstractDownloader> downloader)
{
    sApp->downloadThread().addTaks(downloader);
}
