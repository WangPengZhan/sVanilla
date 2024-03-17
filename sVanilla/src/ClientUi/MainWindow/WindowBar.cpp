#include <QButtonGroup>
#include "ui_WindowBar.h"
#include "WindowBar.h"

WindowBar::WindowBar(QWidget* parent)
    : QFrame(parent)
    , ui(new Ui::WindowBar)
{
    ui->setupUi(this);
    signalsAndSlots();
    const QStringList horizonNavigation({QStringLiteral(":/icon/bar/home.svg"), QStringLiteral(":/icon/bar/video.svg"),
                                         QStringLiteral(":/icon/bar/download.svg"), QStringLiteral(":/icon/bar/setting.svg")});
    ui->Hit->setItemList(horizonNavigation);
    ui->Hit->setColumnWidth(45);
    ui->Hit->setFixedWidth(horizonNavigation.length() * ui->Hit->columnWidth());
    connect(ui->Hit, &VanillaStyle::ToggleButton::currentItemChanged, this, &WindowBar::BarBtnClick);
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
}
