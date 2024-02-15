#include "ui_Toast.h"
#include "Toast.h"
#include <QPropertyAnimation>
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
    auto toast = new Toast(parent);
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
            deleteLater();  //关闭后析构
        });
    });

}
