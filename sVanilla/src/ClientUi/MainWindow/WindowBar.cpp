#include <QButtonGroup>
#include "ui_WindowBar.h"
#include "WindowBar.h"
#include "ClientUi/Event.h"

WindowBar::WindowBar(QWidget* parent)
    : QFrame(parent),
      ui(new Ui::WindowBar),
      m_barBtnGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    SignalsAndSlots();
    m_barBtnGroup->addButton(ui->HomeBtn, 0);
    m_barBtnGroup->addButton(ui->GalleryBtn, 1);
    m_barBtnGroup->addButton(ui->DownloadBtn, 2);
    m_barBtnGroup->addButton(ui->SettingBtn, 3);
//    ui->BarListWidget->item(0)->setSelected(true);
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
    connect(m_barBtnGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* button) {
        int id = m_barBtnGroup->id(button);
        emit Event::getInstance() -> BarBtnClick(id);
    });
//    connect(ui->BarListWidget, &QListWidget::currentRowChanged, Event::getInstance(), &Event::BarBtnClick);

}
