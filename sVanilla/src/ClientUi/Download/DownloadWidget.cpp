#include <QStandardPaths>

#include "DownloadWidget.h"
#include "ui_DownloadWidget.h"
#include "UiDownloader.h"
#include "Download/BiliDownloader.h"
#include "SUI/Tips/Toast.h"
#include "ClientUi/VideoList/VideoData.h"
#include "BiliApi/BilibiliClient.h"
#include "ClientUi/VideoList/VideoData.h"
#include "Util/UrlProcess.h"
#include "ClientUi/Utils/RunTask.h"
#include "Adapter/BaseVideoView.h"

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
    auto uiDownloader = std::make_shared<UiDownloader>(biliDownlaoder);
    addTaskITem(biliDownlaoder, uiDownloader);
}

void DownloadWidget::addDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo, download::ResourseInfo info)
{
    auto biliDownlaoder = std::make_shared<download::BiliDownloader>(info);
    auto uiDownloader = std::make_shared<UiDownloader>(biliDownlaoder, videoInfo);
    uiDownloader->setStatus(UiDownloader::Ready);

    addTaskITem(biliDownlaoder, uiDownloader);
}

void DownloadWidget::addFinishedItem(std::shared_ptr<VideoInfoFull> videoInfo)
{
    ui->listWidgetDownloaded->addDownloadedItem(videoInfo);
}

void DownloadWidget::getBiliUrl(const std::shared_ptr<VideoInfoFull>& videoInfo)
{
    // for sqlite

    auto taskFunc = [this, videoInfo]() {
        return biliapi::BilibiliClient::globalClient().getPlayUrl(std::stoll(videoInfo->videoView->VideoId), 32, videoInfo->videoView->Identifier);
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
    info.option.out = util::FileHelp::removeSpecialChar(videoInfo->videoView->Title) + ".mp4";
    info.option.dir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString();
    const std::list<std::string> h = {"Referer: https://www.bilibili.com"};
    info.option.header = h;

    emit sigDownloadTask(videoInfo, info);
}

void DownloadWidget::setUi()
{
    const QStringList horizonNavigationBtn = {tr("Downloading"), tr("Downloaded")};
    ui->horizonNavigation->setItemList(horizonNavigationBtn);
    ui->horizonNavigation->setUseIcon(false);
    constexpr int horizonNavigationWidth = 120;
    ui->horizonNavigation->setColumnWidth(horizonNavigationWidth);
    connect(ui->horizonNavigation, &Vanilla::ToggleButton::currentItemChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);
}

void DownloadWidget::signalsAndSlots()
{
    connect(this, &DownloadWidget::sigDownloadTask, this, &DownloadWidget::addDownloadTask);
    connect(ui->btnStartAll, &QPushButton::clicked, ui->DownloadListWidget, &DownloadingListWidget::startAll);
    connect(ui->btnStopAll, &QPushButton::clicked, ui->DownloadListWidget, &DownloadingListWidget::stopAll);
    connect(ui->btnDeleteAll, &QPushButton::clicked, ui->DownloadListWidget, &DownloadingListWidget::deleteAll);
    connect(ui->DownloadListWidget, &DownloadingListWidget::finished, this, &DownloadWidget::addFinishedItem);
}

void DownloadWidget::addTaskITem(const std::shared_ptr<download::BiliDownloader>& biliDownloader, const std::shared_ptr<UiDownloader>& uiDownloader)
{
    std::string fullPath;
    if (!biliDownloader->path().empty() && (biliDownloader->path().back() != '/' || biliDownloader->path().back() != '\\'))
    {
        fullPath = biliDownloader->path() + "/" + biliDownloader->filename();
    }
    else
    {
        fullPath = biliDownloader->path() + biliDownloader->filename();
    }
    uiDownloader->setFileName(fullPath);

    ui->DownloadListWidget->addDownloadItem(uiDownloader);
    m_downloadManager->addItem(uiDownloader);
}
