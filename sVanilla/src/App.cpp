#include <QApplication>
#include <sstream>
#include <utility>
#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/Task.h"
#include "ClientUi/Config/SingleConfig.h"
#include "Aria2Net/AriaServer/AriaServer.h"
#include "App.h"

void App::init()
{
    setHighDpi();
    downloadManager = std::make_shared<DownloadManager>();

    maimWindow = std::make_unique<MainWindow>();
    maimWindow->show();
    // MainWindow maimWindow;
    // maimWindow.show();
    // signalsAndSlots();
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

    connect(maimWindow.get(), &MainWindow::AddUri, this, &App::addUri);
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
            maimWindow->updateAria2Version(std::make_shared<aria2net::AriaVersion>(result));
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
            maimWindow->updateDownloadStatus(status);
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
                maimWindow->AddDownloadTask(result.result);
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

void App::updateHomeMsg(const std::string& msg) const
{
    maimWindow->updateHomeMsg(msg);
}
