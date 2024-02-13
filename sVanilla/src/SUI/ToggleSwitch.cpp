#include "ToggleSwitch.h"
#include "Theme/color.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QSvgRenderer>

static constexpr int s_height = 26;
static constexpr int s_innerMargin = 2;
static constexpr int s_iconInnerMargin = 4;
static constexpr int s_handleSize = s_height - s_innerMargin * 2;
static constexpr int s_width = s_handleSize * 2.5 + s_innerMargin * 2;

ToggleSwitch::ToggleSwitch(QWidget* parent)
    : QWidget(parent)
    , m_animation(new QPropertyAnimation(this, "switchPosition"))
{
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_Hover);     // repaint on mouse-enter/mouse-exit
    m_animation->setDuration(400);  // Set duration to 200 ms
    m_animation->setStartValue(0);
    m_animation->setEndValue(100);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
}
void ToggleSwitch::setChecked(bool checked)
{
    if (m_checked == checked)
    {
        return;
    }
    m_checked = checked;
    if (m_checked)
    {
        m_animation->setDirection(QPropertyAnimation::Forward);
    }
    else
    {
        m_animation->setDirection(QPropertyAnimation::Backward);
    }

    m_animation->start();
    emit toggled(checked);
    // update();
}
bool ToggleSwitch::isChecked() const
{
    return m_checked;
}
void ToggleSwitch::toggle()
{
    setChecked(!m_checked);
}
int ToggleSwitch::switchPosition() const
{
    return m_switchPosition;
}
void ToggleSwitch::setSwitchPosition(int pos)
{
    m_switchPosition = pos;
    update();
}
QSize ToggleSwitch::sizeHint() const
{
    return {s_width, s_height};
}
void ToggleSwitch::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);

    QPainterPath handle;
    auto firstRect = QRect(rect().x() + s_innerMargin, rect().y() + s_innerMargin, s_handleSize, s_handleSize);
    const auto secondRect = firstRect.adjusted(firstRect.width() * 3 / 2, 0, firstRect.width() * 3 / 2, 0);

    const int handlePos = rect().x() + s_innerMargin + (rect().width() - s_innerMargin * 2 - s_handleSize) * m_switchPosition / 100;
    const auto handleRect = QRect(handlePos, rect().y() + s_innerMargin, s_handleSize, s_handleSize);
    handle.addRoundedRect(QRectF(handleRect), 5, 5);

    painter.fillPath(handle, QBrush(QColor(200, 200, 200)));

    QSvgRenderer grid;
    grid.load(QString(":/icon/video/grid.svg"));
    grid.render(&painter, QRectF(firstRect).adjusted(s_iconInnerMargin, s_iconInnerMargin, -s_iconInnerMargin, -s_iconInnerMargin));

    QSvgRenderer list;
    firstRect.moveLeft(firstRect.width() * 2);
    list.load(QString(":/icon/video/list.svg"));
    list.render(&painter, QRectF(secondRect).adjusted(s_iconInnerMargin, s_iconInnerMargin, -s_iconInnerMargin, -s_iconInnerMargin));
    painter.restore();
}
void ToggleSwitch::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_mouseDown = true;
    }
    else
    {
        event->ignore();
    }
}
void ToggleSwitch::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_mouseDown)
    {
        m_mouseDown = false;
        toggle();
        emit checked(m_checked);
    }
    else
    {
        event->ignore();
    }
}