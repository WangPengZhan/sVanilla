#pragma once
#include <QApplication>

#include "Aria2Net/AriaServer/AriaServer.h"

class SApplication : public QApplication
{
    Q_OBJECT
public:
    SApplication(int& argc, char** argv);
    ~SApplication();

    aria2net::AriaServer& ariaServer();

private:
    void startServer();

private:
    aria2net::AriaServer m_ariaServer;
};

#define sApp static_cast<SApplication*>(qApp)
