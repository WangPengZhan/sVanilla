#include <QDir>
#include <QPushButton>
#include <QStandardPaths>
#include <QLabel>

#include "Download/AbstractDownloader.h"
#include "Adapter/BilibiliVideoView.h"
#include "BiliApi/BilibiliClient.h"
#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/Task.h"
#include "Util/CoverUtil.h"
#include "SUI/Tips/Toast.h"
#include "VideoGridWidget.h"
#include "VideoWidget.h"
#include "ui_VideoWidget.h"

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoPage)
{
    ui->setupUi(this);
    signalsAndSlots();
    ui->VideoStackedPage->setCurrentWidget(ui->VideoGrid);
    const QStringList horizonNavigation({QStringLiteral(":/icon/video/grid.svg"), QStringLiteral(":/icon/video/list.svg")});
    ui->SwitchBtn->setColumnWidth(45);
    ui->SwitchBtn->setItemList(horizonNavigation);
    connect(ui->SwitchBtn, &Vanilla::ToggleButton::currentItemChanged, ui->VideoStackedPage, &QStackedWidget::setCurrentIndex);
    connect(ui->VideoStackedPage, &QStackedWidget::currentChanged, this, [this]() {
        ui->VideoGrid->widget(1)->hide();
        ui->VideoList->widget(1)->hide();
    });
    ui->VideoGridWidget->getSignalPointer(ui->VideoGrid);
    ui->VideoListWidget->getSignalPointer(ui->VideoList);
#if 0
    for (int i = 0; i < 10; i++)
    {
        Adapter::BaseVideoView view = {std::to_string(i)};
        const auto videoView = std::make_shared<Adapter::BaseVideoView>(view);
        updateVideoItem(videoView);
    }
#endif
}

VideoWidget::~VideoWidget()
{
    delete ui;
}

void VideoWidget::signalsAndSlots()
{
    connect(ui->VideoGridWidget, &VideoGridWidget::downloandBtnClick, this, &VideoWidget::prepareDownloadTask);
}

void VideoWidget::loadBiliViewView(const std::string& uri)
{
    // 1. get video view
    prepareBiliVideoView(uri);

    // 2. loading spinner(Temporarily use toast)
    Toast::Show("start loading video view...");
}

void VideoWidget::prepareBiliVideoView(const std::string& uri)
{
    auto taskFunc = [this, uri]() {
        return biliapi::BilibiliClient::globalClient().getVideoView(uri);
    };
    auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    connect(task.get(), &SignalReturnTask::result, this, [this](const std::any& res) {
        try
        {
            const auto& result = std::any_cast<biliapi::VideoViewOrigin>(res);
            if (result.code != 0)
            {
                return;
            }
            prepareVideoItem(std::make_shared<biliapi::VideoViewOrigin>(result));
        }
        catch (const std::bad_any_cast& e)
        {
        }
    });
    ThreadPool::instance().enqueue(task);
}

void VideoWidget::prepareVideoItem(const std::shared_ptr<biliapi::VideoViewOrigin>& videoView)
{
    // after get video view:
    // 1. add 'download cover image' task
    // 2. add video item
    const QString tempPath = QDir::tempPath();  // It is now in the temporary area
    const auto view = ConvertVideoView(videoView->data);
    totalCoverSize = view.size();
    for (const auto& video : view)
    {
        downloadCover({video.Cover, video.Identifier, tempPath.toStdString()});
        addVideoItem(video.Identifier);
    }
    // after cover ready:
    // 1. stop spinner
    // 2. update video item
    connect(this, &VideoWidget::coverReady, this, [this, view]() {
        Toast::Show("stop loading spinner");  // Temporarily use toast
        for (const auto& video : view)
        {
            updateVideoItem(video);
        }
    });
}

void VideoWidget::downloadCover(const CoverInfo& coverInfo)
{
    auto taskFunc = [coverInfo]() {
        return downloadCoverImage(coverInfo);
    };
    auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    connect(task.get(), &SignalReturnTask::result, this, [this](const std::any& res) {
        try
        {
            const auto& result = std::any_cast<bool>(res);
            if (!result)
            {
                return;
            }
            currentCoverSize++;
            if (currentCoverSize == totalCoverSize)
            {
                emit coverReady();
                currentCoverSize = 0;
            }
        }
        catch (const std::bad_any_cast& e)
        {
        }
    });
    ThreadPool::instance().enqueue(task);
}

void VideoWidget::addVideoItem(const std::string& identifier) const
{
    ui->VideoGridWidget->addVideoItem(identifier);
    ui->VideoListWidget->addVideoItem(identifier);
}

void VideoWidget::updateVideoItem(const Adapter::BaseVideoView& videoView) const
{
    const auto view = std::make_shared<Adapter::BaseVideoView>(videoView);
    ui->VideoGridWidget->updateVideoItem(view);
    ui->VideoListWidget->updateVideoItem(view);
}

void VideoWidget::prepareDownloadTask(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    m_currentView = videoView;
    getBiliUrl();
}

void VideoWidget::getBiliUrl()
{
    auto taskFunc = [this]() {
        return biliapi::BilibiliClient::globalClient().getPlayUrl(std::stoll(m_currentView->VideoId), 64, m_currentView->Identifier);
    };
    auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    connect(task.get(), &SignalReturnTask::result, this, [this](const std::any& res) {
        try
        {
            const auto& result = std::any_cast<biliapi::PlayUrlOrigin>(res);
            if (result.code != 0)
            {
                return;
            }
            praseBiliDownloadUrl(result);
        }
        catch (const std::bad_any_cast& e)
        {
        }
    });
    ThreadPool::instance().enqueue(task);
}

void VideoWidget::praseBiliDownloadUrl(const biliapi::PlayUrlOrigin& playUrl)
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
    info.option.out = m_currentView->Title;
    // Temporarily use the download directory
    info.option.dir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString();
    const std::list<std::string> h = {"Referer: https://www.bilibili.com"};
    info.option.header = h;
    emit createBiliDownloadTask(info);
}

void VideoWidget::clearVideo() const
{
    ui->VideoGridWidget->clearVideo();
    ui->VideoListWidget->clearVideo();
}