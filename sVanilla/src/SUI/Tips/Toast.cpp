#include "ui_Toast.h"
#include "Toast.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QSvgRenderer>
#include <QTimer>

std::unique_ptr<Toast> Toast::instance;

Toast::Toast(QWidget* parent)
    : QWidget(parent)
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
    instance = std::unique_ptr<Toast>(new Toast(parent));
}

void Toast::setUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    timer->setInterval(3000);
    if (const auto window = windowObj())
    {
        this->setParent(window);
    }
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
        // auto animation = new QPropertyAnimation(this, "pos");
        // animation->setDuration(500);
        // animation->setEasingCurve(QEasingCurve::InCubic);
        // animation->setStartValue(QPoint(this->width(), -this->y()));
        // animation->setEndValue(QPoint(this->x(), this->y()));
        // animation->start();
        timer->start();
    }
    else
    {
        hideMessage();
    }
}
void Toast::hideMessage()
{
    timer->stop();
    hide();
    // const auto closeAnimation = new QPropertyAnimation(this, "pos");
    // closeAnimation->setDuration(500);
    // closeAnimation->setStartValue(QPoint(this->x(), this->y()));
    // closeAnimation->setEndValue(QPoint(this->height(), -this->y()));
    // closeAnimation->start();
    // connect(closeAnimation, &QPropertyAnimation::finished, [&] {
        // hide();
    // });
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
QWidget* Toast::windowObj()
{
    for (auto topLevelWidgets = QApplication::topLevelWidgets(); const auto widget : topLevelWidgets)
    {
        if (widget->isWindow() && widget->isVisible() && widget->objectName() == "MainWindow")
        {
            return widget;
        }
    }
    return nullptr;
}
void Toast::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    painter.setClipPath(path);
    painter.setBrush(QBrush(QColor(249, 247, 247)));
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
