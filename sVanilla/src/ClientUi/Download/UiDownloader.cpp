#include <QUuid>

#include "UiDownloader.h"
#include "Storage/StorageManager.h"
#include "Storage/FinishedItemStorage.h"
#include "Storage/DownloadingItemStorage.h"
#include "VideoList/VideoData.h"
#include "Adapter/BaseVideoView.h"

UiDownloader::UiDownloader(std::shared_ptr<AbstractDownloader> downloader, std::shared_ptr<VideoInfoFull> videoInfoFull, QObject* parent)
    : QObject(parent)
    , m_realDownloader(std::move(downloader))
    , m_videoInfoFull(std::move(videoInfoFull))
    , m_storageManager(sqlite::StorageManager::intance())
{
    setStatus(Waitting);
    m_guid = m_videoInfoFull->getGuid();
    createDbDownloadingItem();
}

UiDownloader::~UiDownloader()
{
}

void UiDownloader::setRealDownloader(const std::shared_ptr<AbstractDownloader>& realDownloader)
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

void UiDownloader::setUri(const std::string& uri)
{
    m_uri = uri;
}

const std::string& UiDownloader::uri() const
{
    return m_uri;
}

std::shared_ptr<VideoInfoFull> UiDownloader::videoInfoFull() const
{
    return m_videoInfoFull;
}

void UiDownloader::start()
{
    m_realDownloader->start();
    setStatus(Downloading);
    updateDbStatus();
}

void UiDownloader::stop()
{
    m_realDownloader->stop();
    setStatus(Stopped);
    deleteDbDownloadingItem();
}

void UiDownloader::pause()
{
    m_realDownloader->pause();
    setStatus(Paused);
    updateDbStatus();
}

void UiDownloader::resume()
{
    m_realDownloader->resume();
    setStatus(Downloading);
    updateDbStatus();
}

void UiDownloader::downloadStatus()
{
    m_realDownloader->downloadStatus();
    emit update(m_realDownloader->info(), QString::fromStdString(m_realDownloader->stage()));
    if (m_realDownloader->status() != Downloading)
    {
        setStatus(m_realDownloader->status());
        updateDbStatus();
    }
}

void UiDownloader::finish()
{
    m_realDownloader->finish();

    deleteDbDownloadingItem();
    ctreateDbFinishedItem();

    emit finished(QString());
}

void UiDownloader::createDbDownloadingItem()
{
    DownloadingItem item;
    item.uniqueId = guid();
    item.pluginType = 0;
    item.filePath = filename();
    item.coverPath = m_videoInfoFull->videoView->Cover;
    item.bvid = m_videoInfoFull->videoView->Identifier;
    item.title = m_videoInfoFull->videoView->Title;
    item.auther = m_videoInfoFull->videoView->Publisher;
    item.url = "https://www.bilibili.com/video/" + item.bvid;
    item.duration = std::stoi(m_videoInfoFull->videoView->Duration);
    item.progress = 0;
    item.status = static_cast<int>(status());
    item.type = 0;
    m_storageManager.downloadingStorage()->insertEntities<DownloadingItem>({item});
}

void UiDownloader::updateDbStatus()
{
    const auto& table = sqlite::TableStructInfo<DownloadingItemStorage::Entity>::self();
    sqlite::ConditionWrapper condition;
    condition.addCondition(table.uniqueId, sqlite::Condition::EQUALS, guid());

    m_storageManager.downloadingStorage()->updateStatus(status(), condition);

    emit statusChanged(status());
}

void UiDownloader::deleteDbDownloadingItem()
{
    const auto& table = sqlite::TableStructInfo<DownloadingItemStorage::Entity>::self();
    sqlite::ConditionWrapper condition;
    condition.addCondition(table.uniqueId, sqlite::Condition::EQUALS, guid());
    m_storageManager.downloadingStorage()->deleteEntities(condition);
}

void UiDownloader::ctreateDbFinishedItem()
{
    FinishedItem item;
    item.uniqueId = guid();
    item.pluginType = 0;
    item.filePath = filename();
    item.coverPath = m_videoInfoFull->videoView->Cover;
    item.bvid = m_videoInfoFull->videoView->Identifier;
    item.title = m_videoInfoFull->videoView->Title;
    item.auther = m_videoInfoFull->videoView->Publisher;
    item.url = "https://www.bilibili.com/video/" + item.bvid;
    item.duration = std::stoll(m_videoInfoFull->videoView->Duration);
    item.type = 0;
    item.fileExist = true;
    m_storageManager.finishedItemStorage()->insertEntities<FinishedItem>({item});
}
