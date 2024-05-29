#pragma once
#include <QApplication>
#include <QFileSystemWatcher>

#include "Aria2Net/AriaServer/AriaServer.h"
#include "Plugin/PluginManager.h"
#include "Translater.h"

class SApplication : public QApplication
{
    Q_OBJECT
public:
    SApplication(int& argc, char** argv);
    ~SApplication();

    void init();

    aria2net::AriaServer& ariaServer();
    plugin::PluginManager& pluginManager();
    Translater& translater();

private:
    void startServer();
    void signalsAndSlots();

private:
    aria2net::AriaServer m_ariaServer;
    plugin::PluginManager m_pluginManager;
    QFileSystemWatcher m_watcher;
    Translater m_translater;
};

#define sApp static_cast<SApplication*>(qApp)
