#pragma once
#include <QWidget>
#include <QTimer>

class SlidingWaitBar : public QWidget
{
    Q_OBJECT

public:
    explicit SlidingWaitBar(QWidget* parent = nullptr);
    ~SlidingWaitBar() = default;

    void start();
    void stop();
    void reStart();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void onTimeout();

private:
    QTimer m_timer;
    int m_offset;
    int m_maxOffset;
    bool m_forward;
};
