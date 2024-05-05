
#include "SApplication.h"
#include "ClientUi/Config/SingleConfig.h"

SApplication::SApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    m_watcher.addPath(applicationDirPath() + "/" + QString::fromStdString(plugin::PluginManager::pluginDir()));
    startServer();
    signalsAndSlots();
}

SApplication::~SApplication()
{
}

aria2net::AriaServer& SApplication::ariaServer()
{
    return m_ariaServer;
}

plugin::PluginManager& SApplication::pluginManager()
{
    return m_pluginManager;
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
