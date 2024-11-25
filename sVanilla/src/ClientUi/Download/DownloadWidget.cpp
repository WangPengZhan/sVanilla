#include <QStandardPaths>
#include <QMenu>
#include <QFileInfo>

#include "DownloadWidget.h"
#include "ui_DownloadWidget.h"
#include "UiDownloader.h"
#include "Download/BiliDownloader.h"
#include "SUI/Tips/Toast.h"
#include "SUI/Tips/ToastTip.h"
#include "VideoList/VideoData.h"
#include "BiliApi/BilibiliClient.h"
#include "Utils/UrlProcess.h"
#include "Utils/RunTask.h"
#include "Adapter/BaseVideoView.h"
#include "Storage/StorageManager.h"
#include "Config/SingleConfig.h"
#include "Download/DownloadedListWidget.h"
#include "Storage/DownloadedItemStorage.h"
#include "Storage/DownloadingItemStorage.h"
#include "ClientLog.h"
#include "const_string.h"

constexpr bool isMp4 = false;

DownloadWidget::DownloadWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadWidget)
    , m_downloadManager(new DownloadManager(this))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
    initHistoryData();
}

DownloadWidget::~DownloadWidget()
{
    delete ui;
}

void DownloadWidget::addTaskItem(const std::list<std::string>& videos, const std::list<std::string>& audios, const std::string& fileName)
{
    // param path: come from config file or custom setting in the future
    auto biliDownlaoder = std::make_shared<download::BiliDownloader>(videos, audios, "output", fileName);
    auto uiDownloader = std::make_shared<UiDownloader>(biliDownlaoder, std::shared_ptr<VideoInfoFull>());
    uiDownloader->setStatus(UiDownloader::Ready);

    addTaskItem(biliDownlaoder, uiDownloader);
}

void DownloadWidget::addDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo, download::ResourseInfo info)
{
    MLogI(svanilla::cDownloadModule, "addDownloadTask: {}", videoInfo->getGuid());
    auto biliDownlaoder = std::make_shared<download::BiliDownloader>(info);
    auto uiDownloader = std::make_shared<UiDownloader>(biliDownlaoder, videoInfo);
    uiDownloader->setStatus(UiDownloader::Ready);

    addTaskItem(biliDownlaoder, uiDownloader);
}

void DownloadWidget::addDownloadedItem(std::shared_ptr<VideoInfoFull> videoInfo)
{
    MLogI(svanilla::cDownloadModule, "addDownloadedItem: {}", videoInfo->getGuid());
    ui->downloadedListWidget->addDownloadedItem(videoInfo);
}

void DownloadWidget::getBiliUrl(const std::shared_ptr<VideoInfoFull>& videoInfo)
{
    bool isDownload = sqlite::StorageManager::intance().isDownloaded(videoInfo->getGuid());
    if (isDownload)
    {
        ToastTip::showTip(tr("video has exist"), ToastTip::Warn);
        MLogW(svanilla::cDownloadModule, "getBiliUrl has exist: {}", videoInfo->getGuid());
        return;
    }

    auto copyedVideoInfo = videoInfo;
    copyedVideoInfo->downloadConfig = std::make_shared<DownloadConfig>(*(videoInfo->downloadConfig));
    auto taskFunc = [this, copyedVideoInfo]() {
        auto& biliClient = biliapi::BilibiliClient::globalClient();
        long long qn = 64;
        if (biliClient.isLogined())
        {
            qn = 80;
        }
        if (false)
        {
            qn = 116;
        }

        long long fnval = isMp4 ? 1 : 16;
        MLogI(svanilla::cDownloadModule, "getPlayUrl has exist: {}, qn: {}, fnval: {}", copyedVideoInfo->getGuid(), qn, fnval);
        return biliClient.getPlayUrl(std::stoll(copyedVideoInfo->videoView->VideoId), qn, copyedVideoInfo->videoView->Identifier, fnval);
    };
    auto callback = [this, copyedVideoInfo](const biliapi::PlayUrlOrigin& result) {
        if (result.code != 0)
        {
            MLogW(svanilla::cDownloadModule, "getPlayUrl error {}, error message: {}", result.code, result.message);
            return;
        }
        praseBiliDownloadUrl(result, copyedVideoInfo);
    };
    runTask(taskFunc, callback, this);
}

void DownloadWidget::praseBiliDownloadUrl(const biliapi::PlayUrlOrigin& playUrl, const std::shared_ptr<VideoInfoFull>& videoInfo)
{
    std::list<std::string> video_urls;
    std::list<std::string> audio_urls;
    if (isMp4)
    {
        const auto& videos = playUrl.data.durl;
        for (const auto& video : videos)
        {
            video_urls.push_back(video.url);
        }
    }
    else
    {
        auto& biliClient = biliapi::BilibiliClient::globalClient();
        long long qn = 64;
        if (biliClient.isLogined())
        {
            qn = 80;
        }
        if (false)
        {
            qn = 116;
        }

        int needDownloadVideoId = 16;
        const auto& videos = playUrl.data.dash.video;
        for (const auto& video : videos)
        {
            if (video.id <= qn && video.id > needDownloadVideoId)
            {
                needDownloadVideoId = video.id;
            }
        }

        MLogI(svanilla::cDownloadModule, "needDownloadVideoId: {}", needDownloadVideoId);
        for (const auto& video : videos)
        {
            if (video.id == needDownloadVideoId)
            {
                video_urls.push_back(video.baseUrl);
            }
        }

        int needDownloadAudioId = 30216;
        const auto& audios = playUrl.data.dash.audio;
        for (const auto& audio : audios)
        {
            if (audio.id > needDownloadAudioId)
            {
                needDownloadAudioId = audio.id;
            }
        }

        MLogI(svanilla::cDownloadModule, "needDownloadAudioId: {}", needDownloadAudioId);
        for (const auto& audio : audios)
        {
            if (audio.id == needDownloadAudioId)
            {
                audio_urls.push_back(util::urlDecode(audio.baseUrl));
            }
        }
    }

    download::ResourseInfo info;
    info.videoUris = video_urls;
    info.audioUris = audio_urls;
    auto fileName = videoInfo->fileName();
    info.option.out = fileName + ".mp4";
    info.option.dir = videoInfo->downloadConfig->downloadDir.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString() :
                                                                         videoInfo->downloadConfig->downloadDir.toStdString();
    const std::list<std::string> h = {"Referer: https://www.bilibili.com", std::string("User-Agent: ") + network::chrome};
    info.option.header = h;

    ToastTip::showTip(tr("video add to download!"), ToastTip::Success);
    emit sigDownloadTask(videoInfo, info);
}

std::shared_ptr<VideoInfoFull> DownloadWidget::downloadingItemToVideoInfoFull(const DownloadingItem& item)
{
    auto res = std::make_shared<VideoInfoFull>();
    res->downloadConfig = std::make_shared<DownloadConfig>();
    QFileInfo info(QString::fromStdString(item.filePath));
    res->downloadConfig->downloadDir = info.absolutePath();
    res->downloadConfig->nameRule = info.completeBaseName();
    res->videoView = std::make_shared<Adapter::BaseVideoView>();
    res->videoView->AlternateId = item.aid;
    res->videoView->VideoId = item.cid;
    res->videoView->Cover = item.coverPath;
    res->videoView->Identifier = item.bvid;
    res->videoView->Title = item.title;
    res->videoView->Publisher = item.auther;
    res->videoView->Duration = std::to_string(item.duration);
    return res;
}

std::shared_ptr<VideoInfoFull> DownloadWidget::finishItemToVideoInfoFull(const DownloadedItem& item)
{
    auto res = std::make_shared<VideoInfoFull>();
    res->downloadConfig = std::make_shared<DownloadConfig>();
    QFileInfo info(QString::fromStdString(item.filePath));
    res->downloadConfig->downloadDir = info.absolutePath();
    res->downloadConfig->nameRule = info.completeBaseName();
    res->videoView = std::make_shared<Adapter::BaseVideoView>();
    res->videoView->AlternateId = item.aid;
    res->videoView->VideoId = item.cid;
    res->videoView->Cover = item.coverPath;
    res->videoView->Identifier = item.bvid;
    res->videoView->Title = item.title;
    res->videoView->Publisher = item.auther;
    res->videoView->Duration = std::to_string(item.duration);
    return res;
}

void DownloadWidget::resizeEvent(QResizeEvent* event)
{
    const auto needHide = this->width() < 650;
    ui->btnFilter->setVisible(!needHide);
    ui->btnSort->setVisible(!needHide);
    ui->lineEditSearch->setVisible(!needHide);
    QWidget::resizeEvent(event);
}

void DownloadWidget::setUi()
{
    const QStringList textList = {tr("Downloading"), tr("Completed")};
    ui->horizonNavigation->setItemList(textList);
    const QStringList iconList({QStringLiteral(":/icon/downloading.svg"), QStringLiteral(":/icon/completed.svg")});
    ui->horizonNavigation->setIconList(iconList);
    constexpr int horizonNavigationWidth = 120;
    ui->horizonNavigation->setColumnWidth(horizonNavigationWidth);
    ui->horizonNavigation->setFixedHeight(30);
    ui->stackedWidget->setCurrentWidget(ui->widgetDownloading);
    ui->downloadingListWidget->setInfoPanelSignal(ui->downloadingInfoWidget);
    ui->downloadedListWidget->setInfoPanelSignal(ui->downloadedInfoWidget);
    createSelectedActionMenu();
}

void DownloadWidget::signalsAndSlots()
{
    connect(ui->horizonNavigation, &Vanilla::ToggleButton::currentItemChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    connect(this, &DownloadWidget::sigDownloadTask, this, &DownloadWidget::addDownloadTask);
    connect(ui->btnStartAll, &QPushButton::clicked, ui->downloadingListWidget, &DownloadingListWidget::startAll);
    connect(ui->btnStopAll, &QPushButton::clicked, ui->downloadingListWidget, &DownloadingListWidget::pauseAll);
    connect(ui->btnDeleteAll, &QPushButton::clicked, ui->downloadingListWidget, &DownloadingListWidget::deleteAll);
    connect(ui->downloadingListWidget, &DownloadingListWidget::finished, this, &DownloadWidget::addDownloadedItem);
    connect(ui->downloadingListWidget, &DownloadingListWidget::downloadingCountChanged, this, [&](int downloading, int downloadError) {
        setDownloadingNumber(downloading);
        setDownloadedNumber(downloadError);
    });
    // connect(ui->downloadedListWidget, &DownloadedListWidget::downloadedCountChanged, this, &DownloadWidget::setDownloadedNumber);

    connect(ui->btnClearAll, &QPushButton::clicked, ui->downloadedListWidget, &DownloadedListWidget::clearAll);
    connect(ui->btnRedownload, &QPushButton::clicked, ui->downloadedListWidget, &DownloadedListWidget::reloadAll);
    connect(ui->btnScaned, &QPushButton::clicked, ui->downloadedListWidget, &DownloadedListWidget::scan);
    connect(ui->downloadedListWidget, &DownloadedListWidget::reloadItem, this, &DownloadWidget::getBiliUrl);
    connect(ui->downloadingListWidget, &DownloadingListWidget::reloadItem, this, &DownloadWidget::getBiliUrl);
}

void DownloadWidget::initHistoryData()
{
    auto taskHistoryDownloading = [this]() {
        auto downloadingStorage = sqlite::StorageManager::intance().downloadingStorage();
        auto downloadingItems = downloadingStorage->lastItems();
        for (const auto& item : downloadingItems)
        {
            getBiliUrl(downloadingItemToVideoInfoFull(item));
        }
    };
    ThreadPool::instance().enqueue(taskHistoryDownloading);
    auto taskHistoryDownloaded = [this]() {
        auto downloadedStorage = sqlite::StorageManager::intance().downloadedtemStorage();
        auto downloadedItems = downloadedStorage->lastItems();
        for (auto it = downloadedItems.rbegin(); it != downloadedItems.rend(); ++it)
        {
            emit ui->downloadingListWidget->finished(finishItemToVideoInfoFull(*it));
        }
    };
    ThreadPool::instance().enqueue(taskHistoryDownloaded);
}

void DownloadWidget::addTaskItem(const std::shared_ptr<download::BiliDownloader>& biliDownloader, const std::shared_ptr<UiDownloader>& uiDownloader)
{
    if (biliDownloader->path().empty())
    {
        return;
    }
    std::string fullPath = biliDownloader->path();
    if (fullPath.back() != '/' && fullPath.back() != '\\')
    {
        fullPath += "/";
    }
    fullPath += biliDownloader->filename();
    uiDownloader->setFileName(fullPath);
    const auto uri = biliDownloader->uris();
    if (!uri.empty())
    {
        uiDownloader->setUri(uri.front());
    }
    ui->downloadingListWidget->addDownloadItem(uiDownloader);
    m_downloadManager->addItem(uiDownloader);
}

void DownloadWidget::createSelectedActionMenu()
{
    const auto& btn = ui->btnSelectionAciton;
    auto* const menu = new QMenu(btn);
    connect(btn, &QPushButton::clicked, [btn, menu]() {
        const QPoint pos = btn->mapToGlobal(QPoint(0, -menu->sizeHint().height()));
        menu->exec(pos);
    });

    auto* startAction = new QAction(tr("Start Selected"), menu);
    menu->addAction(startAction);
    connect(startAction, &QAction::triggered, ui->downloadingListWidget, &DownloadingListWidget::startSelectedItem);

    auto* pauseAction = new QAction(tr("Pause Selected"), menu);
    menu->addAction(pauseAction);

    auto* deleteAction = new QAction(tr("Delete Selected"), menu);
    menu->addAction(deleteAction);
    connect(deleteAction, &QAction::triggered, ui->downloadingListWidget, &DownloadingListWidget::deleteSelectedItem);
}

void DownloadWidget::setDownloadingNumber(int number)
{
    ui->widgetDownNumber->setDownloadingNumber(number);
    emit downloadingCountChanged(number);
}

void DownloadWidget::setDownloadedNumber(int number)
{
    ui->widgetDownNumber->setDownloadErrorNumber(number);
    emit downloadedCountChanged(number);
}
