#pragma once
#include <QObject>

#include <memory>

#include "ClientUi/MainWindow/MainWindow.h"
#include "ClientUi/Download/DownloadManager.h"
#include <Aria2Net/AriaClient/AriaClient.h>

// for platform

class PlatformInitializer;
class AppInitializer final : public QObject
{
public:
    AppInitializer();
    ~AppInitializer();

    void initApplicationBefore();
    void init();
    void deinit();

    void signalsAndSlots();
    static void setHighDpi();

    void updateAria2Version();
    void updateDownloadStatus(const std::string& gid);
    void addUri(const std::string& uris);

private:
    void updateHomeMsg(const std::string& msg) const;

private:
    std::unique_ptr<PlatformInitializer> m_initializer;
    std::unique_ptr<MainWindow> maimWindow;
    std::shared_ptr<DownloadManager> downloadManager;
    aria2net::AriaSendOption option;
    aria2net::AriaClient& ariaClient = aria2net::AriaClient::globalClient();

    bool isConnect = false;
};
