#include "Event.h"
Event* Event::m_appEvent_ptr=nullptr;
std::mutex Event::m_mutex;

Event* Event::getInstance()
{
    if (nullptr == m_appEvent_ptr){
        m_mutex.lock();
        m_appEvent_ptr = new Event;
        m_mutex.unlock();
    }
    return m_appEvent_ptr;
}
Event::Event(QObject *parent) : QObject(parent)
{
}