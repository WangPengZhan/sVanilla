#include <QApplication>
#include <sstream>
#include <utility>
#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/Task.h"
#include "ClientUi/Config/SingleConfig.h"
#include "Aria2Net/AriaServer/AriaServer.h"
#include "App.h"
#include "Adapter/BilibiliVideoView.h"
#include "BiliApi/BilibiliClient.h"
#include <QDir>
#include <QStandardPaths>

void App::init()
{
    setHighDpi();
    downloadManager = std::make_shared<DownloadManager>();

    maimWindow = std::make_unique<MainWindow>();
    maimWindow->show();
    signalsAndSlots();
    startAriaServer();

    // option.dir = SingleConfig::instance().getAriaConfig().downloadDir;
    const QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    option.dir = downloadPath.toStdString();

}

void App::setHighDpi()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
    qputenv("QT_DPI_ADJUSTMENT_POLICY", "AdjustDpi");
#endif  // Qt < 6.2.0

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#endif  // Qt < 6.0.0

    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
}

void App::startAriaServer()
{
    if (const auto aria2Config = SingleConfig::instance().getAriaConfig(); !aria2Config.isRemote)
    {
        aria2net::AriaServer ariaServer;
        PRINT("start aria2 local server")
        ariaServer.setErrorFunc([] {});
        ariaServer.startServerAsync();
    }
}

void App::signalsAndSlots()
{
    connect(maimWindow.get(), &MainWindow::AddUri, this, &App::parseUri);
    connect(maimWindow.get(), &MainWindow::onSettingPage, this, &App::updateAria2Version);
    connect(downloadManager.get(), &DownloadManager::toRuquestStatus, this, &App::updateDownloadStatus);
    connect(maimWindow.get(), &MainWindow::downloadBtnClick, this, &App::addDownloadTask);
}

void App::updateAria2Version()
{
    auto taskFunc = [this]() {
        return ariaClient.GetAriaVersionAsync();
    };
    auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    connect(task.get(), &SignalReturnTask::result, this, [this](const std::any& res) {
        try
        {
            auto result = std::any_cast<aria2net::AriaVersion>(res);
            if (!result.id.empty() && result.error.message.empty())
            {
                isConnect = true;
            }
            maimWindow->updateAria2Version(std::make_shared<aria2net::AriaVersion>(result));
        }
        catch (const std::bad_any_cast& e)
        {
            updateHomeMsg(e.what());
        }
    });
    ThreadPool::instance().enqueue(task);
}
void App::updateDownloadStatus(const std::string& gid)
{
    auto taskFunc = [this, gid]() {
        return ariaClient.TellStatus(gid);
    };
    const auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    connect(task.get(), &SignalReturnTask::result, this, [this](const std::any& res) {
        try
        {
            const auto& result = std::any_cast<aria2net::AriaTellStatus>(res);
            const auto status = std::make_shared<aria2net::AriaTellStatus>(result);
            const auto error = downloadManager->updateDownloaderStatus(status);
            if (const auto e = error.message; !e.empty())
            {
                updateHomeMsg(e);
            }
            else
            {
                const auto g = status->result.gid;
                if (downloadManager->neededUpdate.count(g) > 0)
                {
                    maimWindow->updateDownloadStatus(status);
                }
            }
        }
        catch (const std::bad_any_cast& e)
        {
            updateHomeMsg(e.what());
        }
    });
    ThreadPool::instance().enqueue(task);
}

void App::addUri(const std::list<std::string>& uris)
{
    auto taskFunc = [this, uris]() {
        return ariaClient.AddUriAsync(uris, option, -1);
    };
    auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    connect(task.get(), &SignalReturnTask::result, this, [this](const std::any& res) {
        try
        {
            const auto& result = std::any_cast<aria2net::AriaAddUri>(res);
            if (!result.result.empty() && result.error.message.empty())
            {
                auto gid = result.result;
                maimWindow->AddDownloadTask(gid);
                downloadManager->addDownloader(gid);
                updateHomeMsg("Add success");
            }
            else
            {
                updateHomeMsg(result.error.message);
            }
        }
        catch (const std::bad_any_cast& e)
        {
            updateHomeMsg(e.what());
        }
    });
    ThreadPool::instance().enqueue(task);
}
void App::parseUri(const std::string& uri)
{
    // if bili
    const std::list<std::string> h = {"Referer: https://www.bilibili.com"};
    option.header = h;
    auto m_biliClient = BiliApi::BilibiliClient::globalClient();
    const auto res = m_biliClient.GetVideoView(uri);
    const auto videoView = ConvertVideoView(res.data);
    for (const auto& video : videoView)
    {
        downloadCover(video.Cover, video.Identifier);
        maimWindow->updateVideoPage(std::make_shared<Adapter::BaseVideoView>(video));
    }
    // const auto playUrl = m_biliClient.GetPlayUrl(res.data.cid, 64, res.data.bvid);
    // std::list<std::string> video_urls;
    // std::list<std::string> audio_urls;
    // if (playUrl.code != 0)
    // {
    //     PRINTS("play url error", playUrl.message)
    //     PRINTS("play url error", playUrl.message)
    //     return;
    // }
    //
    // const auto videos = playUrl.data.durl;
    // PRINTS("accept_format: ", playUrl.data.accept_format)
    // for (const auto& video : videos)
    // {
    //     video_urls.push_back(video.url);
    //     PRINTS("video url", video.url)
    // }
    // if (!video_urls.empty())
    // {
    //     option.out = res.data.title + ".mp4";
    //     addUri(video_urls);
    // }
}
void App::addDownloadTask(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    const auto playUrl = biliClient.GetPlayUrl(std::stoll(videoView->VideoId), 64, videoView->Identifier);
    std::list<std::string> video_urls;
    std::list<std::string> audio_urls;
    if (playUrl.code != 0)
    {
        PRINTS("play url error", playUrl.message)
        PRINTS("play url error", playUrl.message)
        return;
    }

    const auto videos = playUrl.data.durl;
    PRINTS("accept_format: ", playUrl.data.accept_format)
    for (const auto& video : videos)
    {
        video_urls.push_back(video.url);
        PRINTS("video url", video.url)
    }
    if (!video_urls.empty())
    {
        const std::list<std::string> h = {"Referer: https://www.bilibili.com"};
        option.header = h;
        option.out = videoView->Title + ".mp4";
        addUri(video_urls);
    }
}

void App::downloadCover(const std::string& url, const std::string& Identifier)
{
    QString tempPath = QDir::tempPath();
    tempPath.append("/").append(QString::fromStdString(Identifier)).append("jpg");
    if (!QFile::exists(tempPath))
    {
        FILE* file = fopen(tempPath.toStdString().c_str(), "wb");
        CNetWork netWork;
        netWork.HttpGet(url, file);
        fclose(file);
    }
}
void App::updateHomeMsg(const std::string& msg) const
{
    maimWindow->updateHomeMsg(msg);
}
