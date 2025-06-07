#include "SlidingWaitBar.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

SlidingWaitBar::SlidingWaitBar(QWidget* parent)
    : QWidget(parent)
    , m_offset(0)
    , m_forward(true)
{
    m_timer.setInterval(30);
    connect(&m_timer, &QTimer::timeout, this, &SlidingWaitBar::onTimeout);
    start();
}

void SlidingWaitBar::start()
{
    m_timer.start();
}
void SlidingWaitBar::stop()
{
    m_timer.start();
}

void SlidingWaitBar::reStart()
{
    m_offset = 0;
    m_forward = true;
    m_timer.start();
}

void SlidingWaitBar::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);

    painter.setBrush(Qt::lightGray);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    painter.setBrush(Qt::yellow);
    int barWidth = width() / 5;
    QRect slider(m_offset, 0, barWidth, height());
    painter.drawRect(slider);
}

void SlidingWaitBar::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    m_maxOffset = width() - width() / 5;
}

void SlidingWaitBar::onTimeout()
{
    int step = 5;
    if (m_forward)
    {
        m_offset += step;
        if (m_offset >= m_maxOffset)
            m_forward = false;
    }
    else
    {
        m_offset -= step;
        if (m_offset <= 0)
            m_forward = true;
    }
    update();
}
