#include <QEvent>

#include "MenuEventFilter.h"

MenuEventFilter::MenuEventFilter(QMenu* menu)
    : QObject(menu)
    , m_menu(menu)
{
    m_menu->installEventFilter(this);
}

bool MenuEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_menu && event->type() == QEvent::MouseButtonRelease)
    {
        auto action = qobject_cast<QMenu*>(watched)->activeAction();
        if (action && action->isCheckable())
        {
            action->trigger();
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

QTimerHelp::QTimerHelp(QObject* parent)
    : QObject(parent)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        m_elapse = true;
    });
}

QTimerHelp::~QTimerHelp()
{
    m_timer.stop();
}
void QTimerHelp::setInterval(int msec)
{
    m_timer.setInterval(msec);
}

void QTimerHelp::start()
{
    m_elapse = false;
    m_timer.start();
}

bool QTimerHelp::isElapse() const
{
    return m_elapse;
}

void QTimerHelp::resetElapse()
{
    m_elapse = false;
}
