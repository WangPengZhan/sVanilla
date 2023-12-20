#pragma once

#include <QObject>

#include <chrono>
#include <thread>
#include <future>

#include <Windows.h>

namespace aria2net
{

class AriaServer : public QObject
{
    Q_OBJECT

public:
    AriaServer(QObject* parent = nullptr);
    ~AriaServer();
    void StartServerAsync();
    void CloseServer();
    void ForceCloseServer();

    PROCESS_INFORMATION m_pi;
};

}  // namespace aria2net
