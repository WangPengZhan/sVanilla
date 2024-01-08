#pragma once
#include "Aria2Net/AriaServer/AriaServer.h"
#include <Aria2Net/AriaClient/AriaClient.h>
#include <QtCore/QObject>

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
    std::shared_ptr<Settings> m_settings;
    aria2net::AriaSendOption option;
    aria2net::AriaClient& ariaClient = aria2net::AriaClient::globalClient();

    bool isConnect = false;
public slots:
    void updateAria2Status();
    void addUri(const std::list<std::string>& uris);
    static void updateHomeMsg(std::string msg);
};
