#include "DownloadManager.h"

void AbstractDownloader::updateStatus(const std::shared_ptr<aria2net::AriaTellStatus>& status)
{
    aria2Status = status;
    if (const auto downloadStatus = status->result.status; downloadStatus == "active")
    {
        m_status = Downloading;
    }
    else if (downloadStatus == "paused")
    {
        m_status = Paused;
    }
    else if (downloadStatus == "complete")
    {
        m_status = Finished;
    }
    else if (downloadStatus == "error")
    {
        m_status = Error;
    }
    else if (downloadStatus == "removed")
    {
        m_status = Stopped;
    }
    else
    {
        m_status = Ready;
    }
}
AbstractDownloader::Status AbstractDownloader::status() const
{
    return m_status;
}
std::shared_ptr<aria2net::AriaTellStatus> AbstractDownloader::getStatus() const
{
    return aria2Status;
}

DownloadManager::DownloadManager(QObject* parent)
    : QObject(parent)
    , timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout, this, &DownloadManager::updateDownloadStatusInterval);
    timer->start(1000);
}
void DownloadManager::addDownloader(const std::string& gid)
{
    neededUpdate.insert(gid);
    const auto downloader = std::make_shared<AbstractDownloader>();
    downloaders.insert(std::make_pair(gid, downloader));
}

aria2net::AriaError DownloadManager::updateDownloaderStatus(const std::shared_ptr<aria2net::AriaTellStatus>& status)
{
    const auto gid = status->result.gid;
    const auto downloader = downloaders[gid];

    if (!status->error.message.empty())
    {
        return status->error;
    }
    downloader->updateStatus(status);
    return {};
}

void DownloadManager::checkStatus(const std::string& gid)
{
    const auto downloader = downloaders[gid];
    if (const auto status = downloader->status(); status == AbstractDownloader::Status::Stopped)
    {
        neededUpdate.erase(gid);
        downloaders.erase(gid);
    }
}

void DownloadManager::updateDownloadStatusInterval()
{
    if (downloaders.empty())
    {
        return;
    }
    for (const auto& [gid, downloader] : downloaders)
    {
        if (downloader->status() == AbstractDownloader::Status::Downloading)
        {
            emit toRuquestStatus(gid);
        }
    }
}
