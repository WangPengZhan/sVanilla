#pragma once
#include "BiliApi/BilibiliClient.h"
#include "ClientUi/MainWindow/MainWindow.h"
#include "ClientUi/Download/DownloadManager.h"
#include <Aria2Net/AriaClient/AriaClient.h>
#include <QtCore/QObject>

// for platform
class App final : public QObject
{
public:
    App() = default;
    void init();
    void signalsAndSlots();
    static void startAriaServer() ;
    static void setHighDpi();


private:
    std::unique_ptr<MainWindow> maimWindow;
    std::shared_ptr<DownloadManager> downloadManager;
    aria2net::AriaSendOption option;
    aria2net::AriaClient& ariaClient = aria2net::AriaClient::globalClient();
    BiliApi::BilibiliClient& biliClient = BiliApi::BilibiliClient::globalClient();
    bool isConnect = false;

    void updateHomeMsg(const std::string& msg) const;
public slots:
    void updateAria2Version();
    void updateDownloadStatus(const std::string& gid);
    void addUri(const std::list<std::string>& uris);
    void parseUri(const std::string& uri);
    void addDownloadTask(const std::shared_ptr<Adapter::VideoView>& videoView);
};
