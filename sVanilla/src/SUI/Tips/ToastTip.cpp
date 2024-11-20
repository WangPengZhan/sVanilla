#include "ToastTip.h"

#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimer>

#include "ui_ToastTip.h"

ToastTip* ToastTip::instance = nullptr;
ToastTip::ToastTip(bool showCloseBtn, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ToastTip)
    , m_bShowCloseBtn(showCloseBtn)
    , m_pFadeAnimation(new QPropertyAnimation(this, "windowOpacity", this))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

ToastTip::~ToastTip()
{
    delete ui;
}

void ToastTip::setLevel(const Level& level)
{
    switch (level)
    {
    case Error:
        ui->iconLabel->setPixmap(QPixmap::fromImage(QImage(":/icon/toast/error.svg")));
        setStyleSheet("#ToastTip { background-color: rgb(255, 246, 245); border: "
                      "1px "
                      "solid rgb(255, 39, 39); border-radius: 4px; }");
        break;
    case Warn:
        ui->iconLabel->setPixmap(QPixmap::fromImage(QImage(":/icon/toast/warn.svg")));
        setStyleSheet("#ToastTip { background-color: rgb(255, 227, 173); border: "
                      "1px "
                      "solid rgb(255, 161, 51); border-radius: 4px; }");
        break;
    case Success:
        ui->iconLabel->setPixmap(QPixmap::fromImage(QImage(":/icon/toast/success.svg")));
        setStyleSheet("#ToastTip { background-color: rgb(225, 251, 232); border: "
                      "1px "
                      "solid rgb(80, 193, 78); border-radius: 4px; }");
        break;
    default:
        break;
    }
}

void ToastTip::setAlertText(const QString& text)
{
    ui->textLabel->setText(text);
    adjustSize();
}

void ToastTip::showTip(const QString& tip, Level level, int showTime, bool isShowCloseBtn)
{
    if (instance)
    {
        instance->sigShowTip(tip, level, showTime, isShowCloseBtn);
    }
}

void ToastTip::create(QWidget* parent)
{
    instance = new ToastTip(false, parent);
}

void ToastTip::mousePressEvent(QMouseEvent* event)
{
    if (!m_bShowCloseBtn)
    {
        setAttribute(Qt::WA_NoMouseReplay);
    }
    return QWidget::mousePressEvent(event);
}

void ToastTip::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        return;
    }

    return QWidget::keyPressEvent(event);
}

void ToastTip::setUi()
{
    m_pFadeAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_pFadeAnimation->setEasingCurve(QEasingCurve::Linear);
    m_pFadeAnimation->setStartValue(1.0);
    m_pFadeAnimation->setEndValue(0);
}

void ToastTip::signalsAndSlots()
{
    connect(m_pFadeAnimation, &QPropertyAnimation::finished, this, &ToastTip::slotAnimationFinished);
    connect(this, &ToastTip::sigShowTip, this, &ToastTip::slotShowTip);
    connect(ui->closeBtn, &QPushButton::clicked, this, [&]() {
        m_pFadeAnimation->stop();
        hide();
    });
}

void ToastTip::slotAnimationFinished()
{
    hide();
}

void ToastTip::slotShowTip(QString tip, Level level, int showTime, bool isShowCloseBtn)
{
    if (m_bShowCloseBtn)
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    }
    else
    {
        ui->closeBtn->hide();
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    }
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::NonModal);
    setAttribute(Qt::WA_ShowWithoutActivating);
    m_pFadeAnimation->stop();
    m_pFadeAnimation->setDuration(showTime);
    setLevel(level);
    setAlertText(tip);
    m_pFadeAnimation->start();
    if (parentWidget())
    {
        move(parentWidget()->mapToGlobal(QPoint((parentWidget()->width() - width()) / 2 - 10, 4)));
    }
    show();
}
