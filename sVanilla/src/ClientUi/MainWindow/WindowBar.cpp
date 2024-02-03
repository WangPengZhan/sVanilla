#include <QButtonGroup>
#include "ui_WindowBar.h"
#include "WindowBar.h"

WindowBar::WindowBar(QWidget* parent)
    : QFrame(parent)
    , ui(new Ui::WindowBar)
{
    ui->setupUi(this);
    signalsAndSlots();
}

WindowBar::~WindowBar()
{
    delete ui;
}

QWidget* WindowBar::GetHitWidget() const
{
    return ui->Hit;
}

void WindowBar::setMinButton(QAbstractButton* btn)
{
    ui->horizontalLayout->insertWidget(0, btn);
    connect(btn, &QAbstractButton::clicked, this, &WindowBar::minimizeRequested);
}
void WindowBar::setMaxButton(QAbstractButton* btn)
{
    ui->horizontalLayout->insertWidget(1, btn);
    connect(btn, &QAbstractButton::clicked, this, &WindowBar::maximizeRequested);
}
void WindowBar::setCloseButton(QAbstractButton* btn)
{
    ui->horizontalLayout->insertWidget(2, btn);
    connect(btn, &QAbstractButton::clicked, this, &WindowBar::closeRequested);
}

void WindowBar::signalsAndSlots()
{
    QList<QPushButton*> btns = ui->Hit->findChildren<QPushButton*>();  // find all btns in Hit widget
    for (int i = 0; i < btns.size(); ++i)
    {
        connect(btns[i], &QPushButton::clicked, [this, i] {
            emit BarBtnClick(i);
        });
    }
}
