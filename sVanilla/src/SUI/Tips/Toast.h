#pragma once
#include <QWidget>
#include <QQueue>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

namespace Ui
{
class Toast;
}

class Toast : public QWidget
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
    static void Show(const QString& msg, Level level = Info);

private:
    explicit Toast(QWidget* parent = nullptr);
    void setUi();
    void signalsAndSlots();
    void showMessage(const QString& msg, Level level);
    void showNextMessage();
    void hideMessage();
    void setText(const QString &msg) const;
    void setLevel(Level level);
    void movePosition();
    static QWidget* windowObj();

    QQueue<std::pair<QString, Level>> m_messageQueue;
    Level m_level = Info;
    QTimer* timer;
    Ui::Toast* ui;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
};
