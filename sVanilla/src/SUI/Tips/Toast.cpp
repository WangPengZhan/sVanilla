#include "ui_Toast.h"
#include "Toast.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QSvgRenderer>
#include <QTimer>

Toast::Toast(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Toast())
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

Toast::~Toast()
{
    delete ui;
}
void Toast::showToast(const QString& msg, Level level, int timeout, QWidget* parent)
{
    const auto toast = new Toast(parent);
    toast->m_level = level;
    toast->setText(msg);
    toast->adjustSize();

    auto topLevelWidgets = QApplication::topLevelWidgets();
    for (auto widget : topLevelWidgets)
    {
        if (widget->isWindow() && widget->isVisible() && widget->objectName() == "MainWindow")
        {
            toast->move((widget->width() - toast->width()) / 2 + widget->pos().x(), 20 + widget->pos().y());
            toast->showWithAnimation(timeout);
            break;
        }
    }
}

void Toast::setUi()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);  // 无边框 无任务栏
    setAttribute(Qt::WA_TranslucentBackground, true);
}

void Toast::signalsAndSlots()
{
}
void Toast::setText(const QString& msg)
{
    ui->label->setText(msg);
    ui->label->resize(ui->label->width() + 20, ui->label->height() + 25);
    ui->label->setStyleSheet("QLabel {margin-left: 20px;}");
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
    if (m_level == Success)
    {
        grid.load(QString(":/icon/common/success.svg"));
    }
    else if (m_level == Info)
    {
        grid.load(QString(":/icon/common/info.svg"));
    }
    else if (m_level == Warn)
    {
        grid.load(QString(":/icon/common/warn.svg"));
    }
    else if (m_level == Error)
    {
        grid.load(QString(":/icon/common/error.svg"));
    }

    grid.render(&painter, iconRect);
}
void Toast::showWithAnimation(const int timeout)
{
    auto animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
    show();
    QTimer::singleShot(timeout, [&] {
        // 结束动画
        const auto closeAnimation = new QPropertyAnimation(this, "windowOpacity");
        closeAnimation->setDuration(500);
        closeAnimation->setStartValue(1);
        closeAnimation->setEndValue(0);
        closeAnimation->start();
        connect(closeAnimation, &QPropertyAnimation::finished, [&] {
            close();
            deleteLater();  // 关闭后析构
        });
    });

}
