#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QScrollBar>
#include <QMenu>
#include <QShortcut>
#include <QContextMenuEvent>
#include <QStyleOption>

#include "ui_DownloadTip.h"
#include "DownloadTip.h"

DownloadTip::DownloadTip(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadTip)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

void DownloadTip::setDragRect(QRect rect)
{
    if (rect == QRect())
    {
        m_rect = parentWidget() ? parentWidget()->rect() : rect;
    }
    else
    {
        m_rect = rect;
    }
}

QRect DownloadTip::dragRect() const
{
    return m_rect;
}

void DownloadTip::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragging = true;
        lastMousePosition = event->pos();
    }

    return QWidget::mousePressEvent(event);
}

void DownloadTip::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragging = false;
    }

    return QWidget::mouseReleaseEvent(event);
}

void DownloadTip::mouseMoveEvent(QMouseEvent* event)
{
    if (dragging)
    {
        QRect moveRect = m_rect == QRect() ? parentWidget()->rect() : m_rect;
        QPoint newPos = pos() + (event->pos() - lastMousePosition);
        if (newPos.x() < moveRect.left())
        {
            newPos.setX(moveRect.left());
        }
        else if (newPos.x() + width() > moveRect.right())
        {
            newPos.setX(moveRect.right() - width());
        }

        if (newPos.y() < moveRect.top())
        {
            newPos.setY(moveRect.right());
        }
        else if (newPos.y() + height() > moveRect.bottom())
        {
            newPos.setY(moveRect.bottom() - height());
        }

        move(newPos);
        lastMousePosition = event->pos();
    }

    return QWidget::mouseMoveEvent(event);
}

void DownloadTip::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    opt.initFrom(this);
#else
    opt.init(this);
#endif
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void DownloadTip::setUi()
{
    if (parentWidget())
    {
        parentWidget()->stackUnder(this);
    }
}

void DownloadTip::signalsAndSlots()
{
}
