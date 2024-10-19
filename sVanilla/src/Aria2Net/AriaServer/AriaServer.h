#pragma once
#include <future>

#include <QString>

class TransceiverForQt;
class QProcess;

namespace aria2net
{

class AriaServer
{
public:
    AriaServer();
    ~AriaServer();

    void startLocalServerAsync();

    void closeServer();
    void forceCloseServer();

    void testServer();

    void setErrorFunc(std::function<void()> func);
    void setCloseFunc(std::function<void()> func);

    void setLogDir(const QString& dir);

private:
    std::future<bool> m_future;
    std::function<void()> m_errorFunc;
    std::function<void()> m_closeFunc;
    std::unique_ptr<QProcess> m_aria2Process;
    std::unique_ptr<TransceiverForQt> m_transceiver;
    QString m_dir;
};

}  // namespace aria2net
