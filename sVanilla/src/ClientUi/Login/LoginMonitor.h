#pragma once
#include <QObject>

#include <atomic>
#include <condition_variable>
#include <thread>
#include <memory>

#include "Login/login.h"
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

class LoginMonitor : public QObject
{
    Q_OBJECT
public:
    LoginMonitor(QObject* parent = nullptr);
    ~LoginMonitor();

    bool setLoginer(std::shared_ptr<AbstractLogin> loginer);
    std::shared_ptr<AbstractLogin> loginer();

    void stop();

signals:
    void sigLoginStatus(AbstractLogin::LoginSatus status);

protected:
    void monitorStatus();

private:
    std::atomic<std::shared_ptr<AbstractLogin>> m_loginer;
    SNullMutex m_waitNullMutex;
    std::atomic<bool> m_stop;
    std::condition_variable_any m_cv;
    std::thread m_thread;
};