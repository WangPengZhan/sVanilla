#include "LoginMonitor.h"

LoginMonitor::LoginMonitor(QObject* parent)
    : QObject(parent)
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

bool LoginMonitor::setLoginer(std::shared_ptr<AbstractLogin> loginer)
{
    if (m_loginer.load())
    {
        return false;
    }

    if (loginer)
    {
        m_loginer = loginer;
        m_cv.notify_one();
        return true;
    }

    return false;
}

std::shared_ptr<AbstractLogin> LoginMonitor::loginer()
{
    return m_loginer;
}

void LoginMonitor::stop()
{
    m_stop = true;
    m_cv.notify_all();
}

void LoginMonitor::monitorStatus()
{
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
            m_loginer = std::shared_ptr<AbstractLogin>();
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
}
