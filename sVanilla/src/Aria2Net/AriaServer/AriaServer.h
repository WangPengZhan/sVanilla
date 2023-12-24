#pragma once

#include <QObject>

#include <chrono>
#include <thread>
#include <future>

//#include <Windows.h>

namespace aria2net
{

class AriaServer : public QObject
{
    Q_OBJECT

public:
    explicit AriaServer(QObject* parent = nullptr);
    ~AriaServer() override;
    void StartServerAsync();
    void CloseServer();

//    PROCESS_INFORMATION m_pi;

    AriaServer(const AriaServer& other) = delete;
    AriaServer& operator=(const AriaServer& other) = delete;
    AriaServer(AriaServer&& other) = delete;
    AriaServer& operator=(AriaServer&& other) = delete;

private:
    std::unique_ptr<QProcess> aria2Process;

};

}  // namespace aria2net