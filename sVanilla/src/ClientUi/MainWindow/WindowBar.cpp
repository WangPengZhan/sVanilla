#include "ui_WindowBar.h"
#include "WindowBar.h"
#include "ClientUi/Event.h"

WindowBar::WindowBar(QWidget* parent)
    : QFrame(parent),
      ui(new Ui::WindowBar)
{
    ui->setupUi(this);
    SignalsAndSlots();
    ui->BarListWidget->item(1)->setSelected(true);
}

WindowBar::~WindowBar()
{
    delete ui;
}

QWidget* WindowBar::GetHitWidget() const
{
    return ui->Hit;
}

void WindowBar::SignalsAndSlots()
{
    connect(ui->BarListWidget, &QListWidget::currentRowChanged, this, [](int row) {
        Event::getInstance()->BarBtnClick(row);
    });
//    connect(ui->EnterDownloadBtn, &QToolButton::clicked, Event::getInstance(), &Event::DownloadClick);
//    connect(ui->AccountBtn, &QToolButton::clicked, Event::getInstance(), &Event::AccountBtnClick);
//    connect(ui->SettingBtn, &QToolButton::clicked, Event::getInstance(), &Event::SettingBtnClick);
//    connect(Event::getInstance(), &Event::StackedPageChanged, this, [this](int index) {
//        if (index != 0)
//        {
//            ui->BarListWidget->item(0)->setHidden(true);
//        } else {
//            ui->BarListWidget->item(0)->setHidden(false);
//        }
//    });
}
