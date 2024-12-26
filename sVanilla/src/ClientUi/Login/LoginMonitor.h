#pragma once
#include <QObject>

#include <atomic>
#include <condition_variable>
#include <thread>
#include <memory>

#include <Login.h>

#include "Util/atomic_shared_ptr.h"

struct SNullMutex
{
    void lock() const
    {
    }
    void unlock() const
    {
    }
};

class LoginProxy;

class LoginMonitor : public QObject
{
    Q_OBJECT
public:
    LoginMonitor(QObject* parent = nullptr);
    ~LoginMonitor();

    bool setLoginer(std::shared_ptr<LoginProxy> loginer);
    std::shared_ptr<LoginProxy> loginer();

    void stop();

signals:
    void sigLoginStatus(AbstractLogin::LoginSatus status);

protected:
    void monitorStatus();

private:
    std::mutex m_mutex;
    std::atomic<std::shared_ptr<LoginProxy>> m_loginer;
    SNullMutex m_waitNullMutex;
    std::atomic<bool> m_stop;
    std::condition_variable_any m_cv;
    std::thread m_thread;
};