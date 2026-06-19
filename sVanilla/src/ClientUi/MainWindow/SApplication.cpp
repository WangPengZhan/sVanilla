#include "SApplication.h"
#include "Config/SingleConfig.h"
#include "Config/GlobalData.h"
#include "Plugin/PluginManager.h"
#include "Plugin/PluginInterface.h"
#include "ClientLog.h"
#include "const_string.h"

#include <QDir>
#include <QStandardPaths>

SApplication::SApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    pluginManager().setPluginConfigDir(appDir().toLocal8Bit().toStdString());
    m_ariaServer.setLogDir(appDir());
    m_translater.setTranslatesDir(QApplication::applicationDirPath() + "/translations");
    m_translater.setLanguage(static_cast<Translater::Language>(SingleConfig::instance().language()));
}

SApplication::~SApplication()
{
    waitForPluginLoadTask();
}

void SApplication::init()
{
    m_watcher.addPath(applicationDirPath() + "/" + QString::fromStdString(plugin::PluginManager::pluginDir()));
    startServer();
    signalsAndSlots();
    m_isLoadingPlugins.store(true);
    m_loadPluginFuture = std::async(std::launch::async, [this]() {
        bool succeeded = true;
        try
        {
            pluginManager().loadPlugins();
            m_pluginInterface.setCookiesForPlugins();
        }
        catch (const std::exception& e)
        {
            succeeded = false;
            MLogE(svanilla::cPluginModule, "load plugins failed, msg: {}", e.what());
        }
        catch (...)
        {
            succeeded = false;
            MLogE(svanilla::cPluginModule, "load plugins failed with unknown exception");
        }

        m_isLoadingPlugins.store(false);
        emit pluginsLoaded(succeeded);
    });
}

bool SApplication::isLoadingPlugins() const
{
    return m_isLoadingPlugins.load();
}

void SApplication::waitForPluginLoadTask() noexcept
{
    if (!m_loadPluginFuture.valid())
    {
        return;
    }

    try
    {
        m_loadPluginFuture.get();
    }
    catch (const std::exception& e)
    {
        MLogE(svanilla::cPluginModule, "plugin loading task failed during shutdown, msg: {}", e.what());
    }
    catch (...)
    {
        MLogE(svanilla::cPluginModule, "plugin loading task failed during shutdown with unknown exception");
    }

    m_isLoadingPlugins.store(false);
}

aria2net::AriaServer& SApplication::ariaServer()
{
    return m_ariaServer;
}

PluginInterface& SApplication::pluginInterface()
{
    return m_pluginInterface;
}

plugin::PluginManager& SApplication::pluginManager()
{
    return m_pluginInterface.pluginManager();
}

Translater& SApplication::translater()
{
    return m_translater;
}

download::DownloadStatusThread& SApplication::downloadThread()
{
    return m_downloadThread;
}

ThreadPool& SApplication::threadPool()
{
    return m_threadPool;
}

QString SApplication::appDir()
{
    QString dir;
    if (QFile::exists(QString(svanilla::installedFile)))
    {
        dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + QString(svanilla::softwareName);
        if (!QDir(dir).exists(dir))
        {
            QDir(dir).mkpath(dir);
        }
    }
    else
    {
        dir = QApplication::applicationDirPath();
    }

    return dir;
}

bool SApplication::isInstalled()
{
    return QFile::exists(QString(svanilla::installedFile));
}

void SApplication::startServer()
{
    const auto aria2Config = SingleConfig::instance().ariaConfig();
    if (!aria2Config.isRemote)
    {
        m_ariaServer.setErrorFunc([] {});
        m_ariaServer.startLocalServerAsync();
    }
}

void SApplication::signalsAndSlots()
{
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [&]() {
        pluginManager().pluginDirFileAdded();
    });
}
