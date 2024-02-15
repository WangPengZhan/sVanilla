#include "RoundImageWidget.h"
#include <QPainterPath>

RoundImageWidget::RoundImageWidget(QWidget* parent, const QPixmap& pixmap)
    : QWidget(parent)
    , m_pixmap(pixmap)
{
    // setAttribute(Qt::WA_OpaquePaintEvent);
}
void RoundImageWidget::setPixmap(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
    update();
}
void RoundImageWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    if (m_pixmap.isNull())
    {
        return;
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // Enable antialiasing
    // Create a rounded rectangle path
    QPainterPath path;
    path.addRoundedRect(0, 0, width()-20, height()-20, 15, 15);
    // Clip the painting area to the path
    painter.setClipPath(path);
    // Draw the image
    painter.drawPixmap(0, 0, width()-20, height()-20, m_pixmap);
}