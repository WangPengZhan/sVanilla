#pragma once
#include "Aria2Net/AriaServer/AriaServer.h"
#include "ClientUi/Download/DownloadManager.h"
#include <Aria2Net/AriaClient/AriaClient.h>
#include <QtCore/QObject>
#include <QTimer>

class App : public QObject
{
public:
    App(int argc, char *argv[]);
    int exec();

    void loadSettings();
    void SignalsAndSlots();
    void startAriaServer() const;
    static void setHighDpi();

private:
    int _argc;
    char **_argv;
    std::shared_ptr<Settings> settings;
    std::shared_ptr<DownloadManager> downloadManager;
    aria2net::AriaSendOption option;
    aria2net::AriaClient& ariaClient = aria2net::AriaClient::globalClient();

    bool isConnect = false;
private slots:
    void updateAria2Status();
    void updateDownloadStatus();
    void addUri(const std::list<std::string>& uris);
    static void updateHomeMsg(std::string msg);
};
