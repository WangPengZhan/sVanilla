#pragma once

#include <QObject>
#include <QMenu>

class MenuEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit MenuEventFilter(QMenu* menu);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QMenu* m_menu{ nullptr };
};
