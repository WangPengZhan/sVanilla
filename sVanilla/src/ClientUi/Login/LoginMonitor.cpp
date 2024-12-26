#include "LoginMonitor.h"
#include "ClientLog.h"
#include "const_string.h"
#include "LoginProxy.h"

#include <BaseVideoView.h>

#include <chrono>

LoginMonitor::LoginMonitor(QObject* parent)
    : QObject(parent)
    , m_loginer()
    , m_stop(false)
    , m_thread(&LoginMonitor::monitorStatus, this)
{
}

LoginMonitor::~LoginMonitor()
{
    stop();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

bool LoginMonitor::setLoginer(std::shared_ptr<LoginProxy> loginer)
{
    if (!loginer)
    {
        return false;
    }

    m_loginer = loginer;
    m_cv.notify_one();
    return true;
}

std::shared_ptr<LoginProxy> LoginMonitor::loginer()
{
    return m_loginer;
}

void LoginMonitor::stop()
{
    MLogI(svanilla::cLoginModule, "monitorStatus stop!");
    m_stop = true;
    m_cv.notify_all();
}

void LoginMonitor::monitorStatus()
{
    MLogI(svanilla::cLoginModule, "monitorStatus start!");
    while (true)
    {
        std::unique_lock<SNullMutex> lk(m_waitNullMutex);
        m_cv.wait(lk, [&]() {
            return m_loginer.load() || m_stop;
        });

        if (m_stop)
        {
            return;
        }

        auto loginStatus = m_loginer.load()->getLoginStatus();
        switch (loginStatus)
        {
        case AbstractLogin::Error:
        case AbstractLogin::Success:
        case AbstractLogin::Timeout:
        {
            emit sigLoginStatus(loginStatus);
            m_loginer.load().reset();
            break;
        }
        case AbstractLogin::NoScan:
        case AbstractLogin::ScanedNoAck:
        default:
            emit sigLoginStatus(loginStatus);
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    MLogI(svanilla::cLoginModule, "monitorStatus exit!");
}
