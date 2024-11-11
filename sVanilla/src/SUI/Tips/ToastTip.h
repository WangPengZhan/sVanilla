#pragma once

#include <QWidget>
#include <QString>
#include <memory>

class QTimer;
class QPropertyAnimation;

namespace Ui
{
class ToastTip;
}

class ToastTip : public QWidget
{
    Q_OBJECT
public:
    enum Level
    {
        Success,
        Info,
        Warn,
        Error,
    };

    explicit ToastTip(bool showCloseBtn = false, QWidget* parent = nullptr);
    ~ToastTip();

    void setLevel(const Level& level);
    void setAlertText(const QString& text);

    static void showTip(const QString& tip, Level level = Success, int showTime = 3000, bool isShowCloseBtn = false);
    static void create(QWidget* parent);

signals:
    void sigShowTip(QString tip, Level level, int showTime = 3000, bool isShowCloseBtn = false);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

    void slotAnimationFinished();
    void slotShowTip(QString tip, Level level, int showTime = 3000, bool isShowCloseBtn = false);

private:
    Ui::ToastTip* ui;
    QPropertyAnimation* m_pFadeAnimation{nullptr};
    bool m_bShowCloseBtn;
    static ToastTip* instance;
};
