#pragma once
#include <QApplication>
#include <QFileSystemWatcher>

#include <future>
#include <atomic>

#include "Aria2Net/AriaServer/AriaServer.h"
#include "ClientUi/Plugin/PluginInterface.h"
#include "Translater.h"
#include "Download/DownloadStatusThread.h"
#include "ThreadPool/ThreadPool.h"

class SApplication : public QApplication
{
    Q_OBJECT
public:
    SApplication(int& argc, char** argv);
    ~SApplication();

    void init();
    bool isLoadingPlugins() const;

    aria2net::AriaServer& ariaServer();
    PluginInterface& pluginInterface();
    plugin::PluginManager& pluginManager();
    Translater& translater();
    download::DownloadStatusThread& downloadThread();
    ThreadPool& threadPool();

    static QString appDir();
    static bool isInstalled();

private:
    void startServer();
    void signalsAndSlots();
    void waitForPluginLoadTask() noexcept;

signals:
    void pluginsLoaded(bool succeeded);

private:
    aria2net::AriaServer m_ariaServer;
    PluginInterface m_pluginInterface;
    std::future<void> m_loadPluginFuture;
    std::atomic_bool m_isLoadingPlugins{false};
    QFileSystemWatcher m_watcher;
    Translater m_translater;
    download::DownloadStatusThread m_downloadThread;
    ThreadPool m_threadPool;
};

#define sApp static_cast<SApplication*>(qApp)
