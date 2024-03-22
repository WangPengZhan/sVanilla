#include "ui_Toast.h"
#include "Toast.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QSvgRenderer>
#include <QTimer>

Toast* Toast::instance;

Toast::Toast(QWidget* parent)
    : QWidget(parent)
    , m_animation(new QPropertyAnimation(this, "pos"))
    , timer(new QTimer(this))
    , ui(new Ui::Toast())
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
    hide();
}

void Toast::Show(const QString& msg, const Level level)
{
    instance->signalShowMessage(msg, level);
}

void Toast::create(QWidget* parent)
{
    instance = new Toast(parent);
}

void Toast::setUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    timer->setInterval(3000);
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::Linear);
}

void Toast::signalsAndSlots()
{
    connect(timer, &QTimer::timeout, this, &Toast::showNextMessage);
    connect(this, &Toast::signalShowMessage, this, &Toast::showMessage);
}

void Toast::showMessage(const QString& msg, const Level level)
{
    m_messageQueue.enqueue(std::make_pair(msg, level));
    if (!timer->isActive())
    {
        showNextMessage();
    }
}

void Toast::showNextMessage()
{
    if (!m_messageQueue.empty())
    {
        auto [text, level] = m_messageQueue.dequeue();
        setText(text);
        setLevel(level);
        adjustSize();
        movePosition();
        show();
        m_animation->setStartValue(geometry().topRight());
        m_animation->setEndValue(geometry().topLeft());
        m_animation->start();
        timer->start();
    }
    else
    {
        hideMessage();
    }
}

void Toast::hideMessage()
{
    m_animation->setStartValue(geometry().topLeft());
    m_animation->setEndValue(geometry().topRight());
    m_animation->start();
    connect(m_animation, &QPropertyAnimation::finished, [&] {
        timer->stop();
        hide();
    });
}

void Toast::setText(const QString& msg) const
{
    ui->label->setText(msg);
    ui->label->resize(ui->label->width() + 20, ui->label->height() + 25);
    ui->label->setStyleSheet("QLabel {margin-left: 20px;}");
}

void Toast::setLevel(const Level level)
{
    m_level = level;
}

void Toast::movePosition()
{
    if (parentWidget())
    {
        move(parentWidget()->width() - width() - 10, 30);
    }
}

void Toast::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    painter.setClipPath(path);
    painter.setBrush(QBrush(QColor(249, 247, 247, 100)));
    painter.drawRect(rect());
    const auto iconRect = QRect(rect().x() + 10, rect().y() + 10, rect().height() - 20, rect().height() - 20);
    QSvgRenderer grid;
    const auto level = QStringList{"success", "info", "warn", "error"}[m_level];
    const QString iconPath = QString(":/icon/common/") + level + ".svg";
    grid.load(iconPath);
    grid.render(&painter, iconRect);
}
void Toast::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        showNextMessage();
    }
}
