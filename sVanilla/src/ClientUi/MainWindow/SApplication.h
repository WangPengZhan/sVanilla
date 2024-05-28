#pragma once
#include <QApplication>
#include <QFileSystemWatcher>

#include "Aria2Net/AriaServer/AriaServer.h"
#include "Plugin/PluginManager.h"

class SApplication : public QApplication
{
    Q_OBJECT
public:
    SApplication(int& argc, char** argv);
    ~SApplication();

    void init();

    aria2net::AriaServer& ariaServer();
    plugin::PluginManager& pluginManager();

private:
    void startServer();
    void signalsAndSlots();

private:
    aria2net::AriaServer m_ariaServer;
    plugin::PluginManager m_pluginManager;
    QFileSystemWatcher m_watcher;
};

#define sApp static_cast<SApplication*>(qApp)
