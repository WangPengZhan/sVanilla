#include <QUuid>

#include "UiDownloader.h"
#include "ClientUi/Storage/StorageManager.h"
#include "ClientUi/Storage/FinishedItemStorage.h"
#include "ClientUi/Storage/DownloadingItemStorage.h"
#include "ClientUi/VideoList/VideoData.h"
#include "Adapter/BaseVideoView.h"

UiDownloader::UiDownloader(std::shared_ptr<AbstractDownloader> downloader, QObject* parent)
    : QObject(parent)
    , m_realDownloader(std::move(downloader))
{
    setStatus(Waitting);
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

void UiDownloader::setVideoInfoFull(std::shared_ptr<VideoInfoFull> videoInfoFull)
{
    m_videoInfoFull = std::move(videoInfoFull);
}

std::shared_ptr<VideoInfoFull> UiDownloader::videoINfoFull() const
{
    return m_videoInfoFull;
}

void UiDownloader::start()
{
    m_realDownloader->start();
    setStatus(Downloading);
}

void UiDownloader::stop()
{
    m_realDownloader->stop();
    setStatus(Waitting);
}

void UiDownloader::pause()
{
    m_realDownloader->pause();
    setStatus(Waitting);
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
    if (m_realDownloader->status() != Downloading)
    {
        setStatus(m_realDownloader->status());
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
    storageManager.finishedItemStorage()->insertEntities<FinishedItem>({item});

    emit finished(QString());
}

void UiDownloader::setGuid(std::string guid)
{
    m_guid = std::move(guid);
}
