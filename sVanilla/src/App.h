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
    aria2net::AriaClient& ariaClient = aria2net::AriaClient::globalClient();
    std::shared_ptr<Settings> m_settings;

private:
    int _argc;
    char **_argv;
    aria2net::AriaSendOption option{};

public slots:
    void updateAria2Status();
};
