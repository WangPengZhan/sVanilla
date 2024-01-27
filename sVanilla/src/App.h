#pragma once
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
    void startAriaServer() const;
    static void setHighDpi();

private:
    std::unique_ptr<MainWindow> maimWindow;
    std::shared_ptr<DownloadManager> downloadManager;
    aria2net::AriaSendOption option;
    aria2net::AriaClient& ariaClient = aria2net::AriaClient::globalClient();

    bool isConnect = false;

    void updateHomeMsg(const std::string& msg) const;
public slots:
    void updateAria2Status();
    void updateDownloadStatus();
    void addUri(const std::list<std::string>& uris);
};
