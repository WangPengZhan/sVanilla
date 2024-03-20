#include <QApplication>
#include <QStandardPaths>

#include <sstream>
#include <utility>

#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/Task.h"
#include "ClientUi/Config/SingleConfig.h"
#include "Aria2Net/AriaServer/AriaServer.h"
#include "MacInitializer.h"
#include "WindowInitializer.h"
#include "LinuxInitializer.h"
#include "AppInitializer.h"

AppInitializer::AppInitializer()
{
#ifdef _WIN32
    m_initializer = std::make_unique<WindowInitializer>();
#elif __linux__
    m_initializer = std::make_unique<LinuxInitializer>();
#elif __APPLE__
    m_initializer = std::make_unique<MacInitializer>();
#endif
}

AppInitializer::~AppInitializer()
{
}

void AppInitializer::initApplicationBefore()
{
    PlatformInitializer::allApplicationBefore();
    m_initializer->initApplicationBefore();
}

void AppInitializer::init()
{
    PlatformInitializer::allInit();
    setHighDpi();
    downloadManager = std::make_shared<DownloadManager>();

    maimWindow = std::make_unique<MainWindow>();
    maimWindow->show();
    signalsAndSlots();

    // option.dir = SingleConfig::instance().getAriaConfig().downloadDir;
    const QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    option.dir = downloadPath.toStdString();

    m_initializer->init();
}

void AppInitializer::deinit()
{
    PlatformInitializer::allDeinit();
    m_initializer->deinit();
}

void AppInitializer::setHighDpi()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
    qputenv("QT_DPI_ADJUSTMENT_POLICY", "AdjustDpi");
#endif  // Qt < 6.2.0

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#endif  // Qt < 6.0.0

    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
}

void AppInitializer::signalsAndSlots()
{
    connect(maimWindow.get(), &MainWindow::AddUri, this, &AppInitializer::addUri);
    connect(maimWindow.get(), &MainWindow::onSettingPage, this, &AppInitializer::updateAria2Version);
}

void AppInitializer::updateAria2Version()
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
void AppInitializer::updateDownloadStatus(const std::string& gid)
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
        }
        catch (const std::bad_any_cast& e)
        {
            updateHomeMsg(e.what());
        }
    });
    ThreadPool::instance().enqueue(task);
}

void AppInitializer::addUri(const std::string& uris)
{
    auto taskFunc = [this, uris]() {
        return ariaClient.AddUriAsync({uris}, option, -1);
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
                // downloadManager->addDownloader(gid);
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

void AppInitializer::updateHomeMsg(const std::string& msg) const
{
    maimWindow->updateHomeMsg(msg);
}
