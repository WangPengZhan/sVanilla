#include <QStandardPaths>
#include <QMenu>

#include "DownloadWidget.h"
#include "ui_DownloadWidget.h"
#include "UiDownloader.h"
#include "Download/BiliDownloader.h"
#include "SUI/Tips/Toast.h"
#include "VideoList/VideoData.h"
#include "BiliApi/BilibiliClient.h"
#include "Utils/UrlProcess.h"
#include "Utils/RunTask.h"
#include "Adapter/BaseVideoView.h"
#include "Storage/StorageManager.h"
#include "Config/SingleConfig.h"
#include "Download/DownloadedListWidget.h"
#include "Storage/FinishedItemStorage.h"

DownloadWidget::DownloadWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadWidget)
    , m_downloadManager(new DownloadManager(this))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
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
    auto biliDownlaoder = std::make_shared<download::BiliDownloader>(info);
    auto uiDownloader = std::make_shared<UiDownloader>(biliDownlaoder, videoInfo);
    uiDownloader->setStatus(UiDownloader::Ready);

    addTaskItem(biliDownlaoder, uiDownloader);
}

void DownloadWidget::addFinishedItem(std::shared_ptr<VideoInfoFull> videoInfo)
{
    ui->downloadedListWidget->addDownloadedItem(videoInfo);
}

void DownloadWidget::getBiliUrl(const std::shared_ptr<VideoInfoFull>& videoInfo)
{
    bool isDownload = sqlite::StorageManager::intance().isDownloaded(videoInfo->getGuid());
    if (isDownload)
    {
        return;
    }

    auto copyedVideoInfo = videoInfo;
    copyedVideoInfo->downloadConfig = std::make_shared<DownloadConfig>(*(videoInfo->downloadConfig));
    auto taskFunc = [this, videoInfo]() {
        return biliapi::BilibiliClient::globalClient().getPlayUrl(std::stoll(videoInfo->videoView->VideoId), 80, videoInfo->videoView->Identifier);
    };
    auto callback = [this, videoInfo](const biliapi::PlayUrlOrigin& result) {
        if (result.code != 0)
        {
            return;
        }
        praseBiliDownloadUrl(result, videoInfo);
    };
    runTask(taskFunc, callback, this);
}

void DownloadWidget::praseBiliDownloadUrl(const biliapi::PlayUrlOrigin& playUrl, const std::shared_ptr<VideoInfoFull>& videoInfo)
{
    std::list<std::string> video_urls;
    std::list<std::string> audio_urls;
    const auto videos = playUrl.data.durl;
    for (const auto& video : videos)
    {
        video_urls.push_back(video.url);
    }
    download::ResourseInfo info;
    info.videoUris = video_urls;
    info.audioUris = audio_urls;
    auto fileName = videoInfo->fileName();
    info.option.out = fileName + ".mp4";
    info.option.dir = videoInfo->downloadConfig->downloadDir.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString() :
                                                                         videoInfo->downloadConfig->downloadDir.toStdString();
    const std::list<std::string> h = {"Referer: https://www.bilibili.com"};
    info.option.header = h;

    emit sigDownloadTask(videoInfo, info);
}

std::shared_ptr<VideoInfoFull> DownloadWidget::finishItemToVideoInfoFull(const FinishedItem& item)
{
    auto res = std::make_shared<VideoInfoFull>();
    res->downloadConfig = std::make_shared<DownloadConfig>();
    res->videoView = std::make_shared<Adapter::BaseVideoView>();
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
    connect(ui->downloadingListWidget, &DownloadingListWidget::finished, this, &DownloadWidget::addFinishedItem);
    connect(ui->downloadingListWidget, &DownloadingListWidget::downloadingCountChanged, this, &DownloadWidget::setDownloadingNumber);
    connect(ui->downloadedListWidget, &DownloadedListWidget::downloadedCountChanged, this, &DownloadWidget::setDownloadedNumber);

    connect(ui->btnClearAll, &QPushButton::clicked, ui->downloadedListWidget, &DownloadedListWidget::clearAll);
    connect(ui->btnRedownload, &QPushButton::clicked, ui->downloadedListWidget, &DownloadedListWidget::reloadAll);
    connect(ui->btnScaned, &QPushButton::clicked, ui->downloadedListWidget, &DownloadedListWidget::scan);
    connect(ui->downloadedListWidget, &DownloadedListWidget::reloadItem, this, &DownloadWidget::getBiliUrl);
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
    emit downloadingCountChanged(number);
}

void DownloadWidget::setDownloadedNumber(int number)
{
    emit downloadedCountChanged(number);
}
