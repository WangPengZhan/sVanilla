
#include "SApplication.h"
#include "ClientUi/Config/SingleConfig.h"

SApplication::SApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    m_translater.setLanguage(static_cast<Translater::Language>(SingleConfig::instance().language()));
}

SApplication::~SApplication()
{
}

void SApplication::init()
{
    m_watcher.addPath(applicationDirPath() + "/" + QString::fromStdString(plugin::PluginManager::pluginDir()));
    startServer();
    signalsAndSlots();
}

aria2net::AriaServer& SApplication::ariaServer()
{
    return m_ariaServer;
}

plugin::PluginManager& SApplication::pluginManager()
{
    return m_pluginManager;
}

Translater& SApplication::translater()
{
    return m_translater;
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
