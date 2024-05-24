#include <QPainter>
#include <QPushButton>

#include "DownloadCount.h"

static constexpr int padding = 3;
static constexpr int radius = 5;
static constexpr int textWidth = 30;
static constexpr int textHeight = 16;

DownloadCount::DownloadCount(QWidget* parent)
    : QWidget(parent)
{
}

int DownloadCount::downloadCount() const
{
    return m_downloadingCount;
}

void DownloadCount::setDownloadingCount(int count)
{
    m_downloadingCount = count;
    update();
}

int DownloadCount::downloadedCount() const
{
    return m_downloadedCount;
}

void DownloadCount::setDownloadedCount(int count)
{
    m_downloadedCount = count;
    update();
}

void DownloadCount::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const auto rect = this->rect().adjusted(1, 3, -1, -3);
    painter.setPen(Qt::lightGray);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect, radius, radius);

    const auto downloadingRect = rect.adjusted(padding, padding, -textWidth - padding, -padding);
    const auto downloadedRect = rect.adjusted(textWidth + padding, padding, -padding, -padding);
    const auto splitLine = rect.center().x();
    painter.drawLine(splitLine, rect.top() + padding, splitLine, rect.bottom() - padding);

    painter.setPen(Qt::black);
    painter.drawText(downloadingRect, Qt::AlignCenter, QString::number(m_downloadingCount));
    painter.drawText(downloadedRect, Qt::AlignCenter, QString::number(m_downloadedCount));
}

QSize DownloadCount::sizeHint() const
{
    return {2 * textWidth + 2 * padding, textHeight};
}