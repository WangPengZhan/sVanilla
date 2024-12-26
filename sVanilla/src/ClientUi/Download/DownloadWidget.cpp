#include <QStandardPaths>
#include <QMenu>
#include <QFileInfo>

#include <BaseVideoView.h>
#include <Download/FileDownloader.h>

#include "DownloadWidget.h"
#include "ui_DownloadWidget.h"
#include "UiDownloader.h"
#include "SUI/Tips/Toast.h"
#include "SUI/Tips/ToastTip.h"
#include "VideoList/VideoData.h"
#include "Utils/UrlProcess.h"
#include "Utils/RunTask.h"
#include "Storage/StorageManager.h"
#include "Config/SingleConfig.h"
#include "Download/DownloadedListWidget.h"
#include "Storage/DownloadedItemStorage.h"
#include "Storage/DownloadingItemStorage.h"
#include "MainWindow/SApplication.h"
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

void DownloadWidget::addDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo)
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
        auto plugin = sApp->pluginManager().getPlugin(copyedVideoInfo->videoView->pluginType);
        if (!plugin)
        {
            return std::shared_ptr<download::FileDownloader>{};
        }
        auto downloader = plugin->getDownloader(*copyedVideoInfo);
        return downloader;
    };
    auto callback = [this, copyedVideoInfo](std::shared_ptr<download::FileDownloader> downloader) {
        addDownloadingItem(downloader, copyedVideoInfo);
    };
    runTask(taskFunc, callback, this);
}

void DownloadWidget::addDownloadingItem(std::shared_ptr<download::FileDownloader> downloader, std::shared_ptr<VideoInfoFull> videoInfo)
{
    MLogI(svanilla::cDownloadModule, "addDownloadTask: {}", videoInfo->getGuid());
    auto uiDownloader = std::make_shared<UiDownloader>(downloader, videoInfo);
    uiDownloader->setStatus(UiDownloader::Ready);

    addDownloadingItem(downloader, uiDownloader);
}

void DownloadWidget::addDownloadedItem(std::shared_ptr<VideoInfoFull> videoInfo)
{
    MLogI(svanilla::cDownloadModule, "addDownloadedItem: {}", videoInfo->getGuid());
    ui->downloadedListWidget->addDownloadedItem(videoInfo);
}

std::shared_ptr<VideoInfoFull> DownloadWidget::downloadingItemToVideoInfoFull(const DownloadingItem& item)
{
    auto res = std::make_shared<VideoInfoFull>();
    res->downloadConfig = std::make_shared<DownloadConfig>();
    QFileInfo info(QString::fromStdString(item.filePath));
    res->downloadConfig->downloadDir = info.absolutePath().toStdString();
    res->downloadConfig->nameRule = info.completeBaseName().toStdString();
    res->videoView = std::make_shared<adapter::BaseVideoView>();
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
    res->downloadConfig->downloadDir = info.absolutePath().toStdString();
    res->downloadConfig->nameRule = info.completeBaseName().toStdString();
    res->videoView = std::make_shared<adapter::BaseVideoView>();
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
    connect(ui->downloadedListWidget, &DownloadedListWidget::reloadItem, this, &DownloadWidget::addDownloadTask);
    connect(ui->downloadingListWidget, &DownloadingListWidget::reloadItem, this, &DownloadWidget::addDownloadTask);
}

void DownloadWidget::initHistoryData()
{
    auto taskHistoryDownloading = [this]() {
        auto downloadingStorage = sqlite::StorageManager::intance().downloadingStorage();
        auto downloadingItems = downloadingStorage->lastItems();
        for (const auto& item : downloadingItems)
        {
            addDownloadTask(downloadingItemToVideoInfoFull(item));
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

void DownloadWidget::addDownloadingItem(const std::shared_ptr<download::FileDownloader>& fileDownloader, const std::shared_ptr<UiDownloader>& uiDownloader)
{
    if (fileDownloader->path().empty())
    {
        return;
    }
    std::string fullPath = fileDownloader->path();
    if (fullPath.back() != '/' && fullPath.back() != '\\')
    {
        fullPath += "/";
    }
    fullPath += fileDownloader->filename();
    uiDownloader->setFileName(fullPath);
    // const auto uri = fileDownloader->uris();
    // if (!uri.empty())
    // {
    //     uiDownloader->setUri(uri.front());
    // }
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
