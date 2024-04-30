#include <QUuid>

#include "UiDownloader.h"
#include "ClientUi/Storage/StorageManager.h"
#include "ClientUi/Storage/FinishedItemStorage.h"
#include "ClientUi/Storage/DownloadingItemStorage.h"
#include "ClientUi/VideoList/VideoData.h"
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

void UiDownloader::setRealDownloader(std::shared_ptr<AbstractDownloader> realDownloader)
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

std::shared_ptr<VideoInfoFull> UiDownloader::videoINfoFull() const
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
    setStatus(Waitting);
    updateDbStatus();
}

void UiDownloader::pause()
{
    m_realDownloader->pause();
    setStatus(Waitting);
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
    auto info = m_realDownloader->info();

    auto& storageManager = sqlite::StorageManager::intance();
    auto& table = sqlite::TableStructInfo<DownloadingItemStorage::Entity>::self();
    sqlite::ConditionWrapper condition;
    condition.addCondition(table.uniqueId, sqlite::Condition::EQUALS, guid());
    storageManager.downloadingStorage()->deleteEntities(condition);

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
    item.duration = std::stoll(m_videoInfoFull->videoView->Duration);
    item.progress = 0;
    item.status = static_cast<int>(status());
    item.type = 0;
    m_storageManager.downloadingStorage()->insertEntities<DownloadingItem>({item});
}

void UiDownloader::updateDbStatus()
{
    auto& table = sqlite::TableStructInfo<DownloadingItemStorage::Entity>::self();
    sqlite::ConditionWrapper condition;
    condition.addCondition(table.uniqueId, sqlite::Condition::EQUALS, guid());

    m_storageManager.downloadingStorage()->updateStatus(status(), condition);
}

void UiDownloader::deleteDbDownloadingItem()
{
    auto& table = sqlite::TableStructInfo<DownloadingItemStorage::Entity>::self();
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
    m_storageManager.finishedItemStorage()->insertEntities<FinishedItem>({item});
}
