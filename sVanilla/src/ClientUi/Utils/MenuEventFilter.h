#pragma once

#include <QObject>
#include <QMenu>
#include <QEvent>
#include <QTimer>

class MenuEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit MenuEventFilter(QMenu* menu);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QMenu* m_menu{nullptr};
};

struct QTimerHelp : public QObject
{
    Q_OBJECT
public:
    explicit QTimerHelp(QObject* parent = nullptr);
    ~QTimerHelp();

    void setInterval(int msec);
    void start();

    bool isElapse() const;
    void resetElapse();

private:
    QTimer m_timer;
    bool m_elapse{false};
};
