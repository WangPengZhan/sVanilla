#include "RoundImageWidget.h"
#include <QPainterPath>
#include <utility>

RoundImageWidget::RoundImageWidget(QWidget* parent, QPixmap  pixmap)
    : QWidget(parent)
    , m_pixmap(std::move(pixmap))
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
    path.addRoundedRect(rect().toRectF(), 15, 15);
    // Clip the painting area to the path
    painter.setClipPath(path);
    // Draw the image
    painter.drawPixmap(rect(), m_pixmap);
}