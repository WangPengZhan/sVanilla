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

private:
    std::unique_ptr<PlatformInitializer> m_initializer;
};
