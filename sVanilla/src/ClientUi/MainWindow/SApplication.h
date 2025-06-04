#pragma once
#include <QApplication>
#include <QFileSystemWatcher>

#include "Aria2Net/AriaServer/AriaServer.h"
#include "Plugin/PluginInterface.h"
#include "Translater.h"
#include "Download/DownloadStatusThread.h"

class SApplication : public QApplication
{
    Q_OBJECT
public:
    SApplication(int& argc, char** argv);
    ~SApplication();

    void init();

    aria2net::AriaServer& ariaServer();
    PluginInterface& pluginInterface();
    plugin::PluginManager& pluginManager();
    Translater& translater();
    download::DownloadStatusThread& downloadThread();

    static QString appDir();
    static bool isInstalled();

private:
    void startServer();
    void signalsAndSlots();

private:
    aria2net::AriaServer m_ariaServer;
    PluginInterface m_pluginInterface;
    QFileSystemWatcher m_watcher;
    Translater m_translater;
    download::DownloadStatusThread m_downloadThread;
};

#define sApp static_cast<SApplication*>(qApp)
