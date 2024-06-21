#include <QPainter>

#include "DividingLine.h"

DividingLine::DividingLine(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void DividingLine::setLineWidth(qreal width)
{
    lineWidth = width;
    setMinimumHeight(lineWidth);
}

QSize DividingLine::sizeHint() const
{
    return {rect().width(), 10};
}

void DividingLine::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.save();
    const QColor color("#B4B4B8");
    painter.setPen(color);
    painter.setBrush(color);
    painter.drawRoundedRect(rect().x(), rect().y(), rect().width(), lineWidth, lineWidth, lineWidth);
    painter.restore();
}