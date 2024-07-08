#include "SApplication.h"
#include "ClientUi/Config/SingleConfig.h"
#include "ClientUi/Config/GlobalData.h"

#include <QDir>
#include <QStandardPaths>

SApplication::SApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    m_pluginManager.setPluginConfigDir(appDir().toLocal8Bit().toStdString());
    m_ariaServer.setLogDir(appDir());
    m_translater.setTranslatesDir(QApplication::applicationDirPath() + "/translations");
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
    pluginManager().loadPlugins();
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

QString SApplication::appDir()
{
    QString dir;
    if (QFile::exists(QString(constant::installedFile)))
    {
        dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + QString(constant::softwareName);
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
    return QFile::exists(QString(constant::installedFile));
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
