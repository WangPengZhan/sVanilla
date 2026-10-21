#include <QUuid>
#include <charconv>
#include <limits>
#include <string_view>

#include <BaseVideoView.h>

#include "UiDownloader.h"
#include "Storage/StorageManager.h"
#include "Storage/DownloadedItemStorage.h"
#include "Storage/DownloadingItemStorage.h"
#include "VideoList/VideoData.h"

namespace
{
int parseDuration(std::string_view duration)
{
    if (duration.empty())
    {
        return 0;
    }
    int64_t total = 0;
    bool hasComponent = false;
    do
    {
        const auto separator = duration.find(':');
        const auto part = duration.substr(0, separator);
        int64_t value = 0;
        const auto [end, error] = std::from_chars(part.data(), part.data() + part.size(), value);
        if (error != std::errc{} || end != part.data() + part.size() || value < 0 || value > std::numeric_limits<int>::max() ||
            total > (std::numeric_limits<int>::max() - value) / (hasComponent ? 60 : 1))
        {
            return 0;
        }
        total = total * (hasComponent ? 60 : 1) + value;
        hasComponent = true;
        if (separator == std::string_view::npos)
        {
            break;
        }
        duration.remove_prefix(separator + 1);
    } while (!duration.empty());
    return duration.empty() ? 0 : static_cast<int>(total);
}
}  // namespace

UiDownloader::UiDownloader(std::shared_ptr<AbstractDownloader> downloader, std::shared_ptr<VideoInfoFull> videoInfoFull, QObject* parent)
    : QObject(parent)
    , m_realDownloader(std::move(downloader))
    , m_videoInfoFull(std::move(videoInfoFull))
    , m_storageManager(sqlite::StorageManager::instance())
{
    setStatus(Waiting);
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
    createDbFinishedItem();

    emit finished(QString());
}

void UiDownloader::createDbDownloadingItem()
{
    DownloadingItem item;
    item.uniqueId = guid();
    item.pluginId = m_videoInfoFull->videoView->pluginId;
    item.filePath = filename();
    item.coverPath = m_videoInfoFull->videoView->Cover;
    item.id = m_videoInfoFull->videoView->Identifier;
    item.idType = m_videoInfoFull->videoView->IdType;
    item.title = m_videoInfoFull->videoView->Title;
    item.author = m_videoInfoFull->videoView->Publisher;
    item.url = uri();
    item.option1 = m_videoInfoFull->videoView->Option1;
    item.option2 = m_videoInfoFull->videoView->Option2;
    item.option3 = m_videoInfoFull->videoView->Option3;
    item.duration = parseDuration(m_videoInfoFull->videoView->Duration);
    item.progress = 0;
    item.status = static_cast<int>(status());
    item.fileType = m_videoInfoFull->videoView->fileType;
    item.fileExtension = m_videoInfoFull->videoView->fileExtension;
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

void UiDownloader::createDbFinishedItem()
{
    DownloadedItem item;
    item.uniqueId = guid();
    item.pluginId = m_videoInfoFull->videoView->pluginId;
    item.filePath = filename();
    item.coverPath = m_videoInfoFull->videoView->Cover;
    item.id = m_videoInfoFull->videoView->Identifier;
    item.idType = m_videoInfoFull->videoView->IdType;
    item.title = m_videoInfoFull->videoView->Title;
    item.author = m_videoInfoFull->videoView->Publisher;
    item.url = uri();
    item.option1 = m_videoInfoFull->videoView->Option1;
    item.option2 = m_videoInfoFull->videoView->Option2;
    item.option3 = m_videoInfoFull->videoView->Option3;
    item.duration = parseDuration(m_videoInfoFull->videoView->Duration);
    item.fileType = m_videoInfoFull->videoView->fileType;
    item.fileExist = true;
    item.fileExtension = m_videoInfoFull->videoView->fileExtension;
    m_storageManager.downloadedItemStorage()->insertEntities<DownloadedItem>({item});
}
