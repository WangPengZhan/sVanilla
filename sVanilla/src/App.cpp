#include <QApplication>

#include <sstream>
#include <utility>

#include "ClientUi/MainWindow/MainWindow.h"
#include "Logger/Dump.h"
#include "Logger/Logger.h"
#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/Task.h"
#include "ClientUi/Config/SingleConfig.h"

#include "App.h"
#include "ClientUi/Event.h"

App::App(const int argc, char** argv)
    : _argc(argc)
    , _argv(argv)
{
}

void App::init()
{
    setHighDpi();
    downloadManager = std::make_shared<DownloadManager>();

    signalsAndSlots();
    startAriaServer();
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

void App::startAriaServer() const
{
    auto aria2Config = SingleConfig::instance().getAriaConfig();
    if (!aria2Config.isRemote)
    {
        aria2net::AriaServer ariaServer;
        PRINT("start aria2 local server")
        ariaServer.setErrorFunc([] {});
        ariaServer.startServerAsync();
    }
}

void App::signalsAndSlots()
{
    // AddUri signal -> Event::AddUri (ui -> core)
    connect(Event::getInstance(), &Event::AddUri, this, &App::addUri);

    /// download interval timer -> Event::IntervalUpdateDownloadStatus (ui -> core)
    connect(Event::getInstance(), &Event::IntervalUpdateDownloadStatus, this, &App::updateDownloadStatus);
}

void App::updateAria2Status()
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
            emit Event::getInstance()->updateAria2Version(std::make_shared<aria2net::AriaVersion>(std::move(result)));
        }
        catch (const std::bad_any_cast& e)
        {
            updateHomeMsg(e.what());
        }
    });
    ThreadPool::instance().enqueue(task);
}

void App::updateDownloadStatus()
{
    const std::list<std::string>& gids = downloadManager->downloadGIDs;
    if (gids.empty())
    {
        return;
    }
    for (const auto& gid : gids)
    {
        const auto status = std::make_shared<aria2net::AriaTellStatus>(ariaClient.TellStatus(gid));
        if (!status->result.gid.empty() && status->error.message.empty())
        {
            emit Event::getInstance()->updateDownloadStatus(status);
        }
    }
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
                downloadManager->addDownloadGID(result.result);
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

void App::updateHomeMsg(std::string msg)
{
    emit Event::getInstance()->updateMsg(std::move(msg));
}
