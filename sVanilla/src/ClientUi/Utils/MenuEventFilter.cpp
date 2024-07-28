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
