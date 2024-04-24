#include "RoundImageWidget.h"
#include <QPainterPath>
#include <utility>
#include <QtWidgets/QVBoxLayout>

RoundImageWidget::RoundImageWidget(QWidget* parent)
    : QWidget(parent)
{
}

void RoundImageWidget::setPixmap(const QPixmap& pixmap)
{
    // m_image->setPixmap(pixmap);
    m_pixmap = pixmap;
    m_originalPixmap = pixmap;
    resizePixmap();
}

void RoundImageWidget::resizePixmap()
{
    if (m_pixmap.isNull())
    {
        return;
    }
    const QSize originalSize = m_pixmap.size();

    const double imgRatio = static_cast<double>(originalSize.width()) / static_cast<double>(originalSize.height());
    const double widgetRatio = static_cast<double>(width()) / static_cast<double>(height());

    int finalWidth;
    int finalHeight;
    if (imgRatio > widgetRatio)
    {
        finalWidth = width();
        finalHeight = static_cast<int>(finalWidth / imgRatio);
    }
    else
    {
        finalHeight = height();
        finalWidth = static_cast<int>(finalHeight * imgRatio);
    }

    const QSize finalSize(finalWidth, finalHeight);
    m_pixmap = m_originalPixmap.scaled(finalSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void RoundImageWidget::resizeEvent(QResizeEvent* event)
{
    resizePixmap();
    QWidget::resizeEvent(event);
}

void RoundImageWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    if (m_pixmap.isNull())
    {
        return;
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    const auto target = rect().center() - QPoint(m_pixmap.width() / 2, m_pixmap.height() / 2);
    const auto source = QRect(target, m_pixmap.size());
    path.addRoundedRect(source, m_radius, m_radius);
    painter.setClipPath(path);
    painter.drawPixmap(target, m_pixmap);
}
