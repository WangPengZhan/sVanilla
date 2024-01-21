#include <QButtonGroup>
#include "ui_WindowBar.h"
#include "WindowBar.h"

WindowBar::WindowBar(QWidget* parent)
    : QFrame(parent),
      ui(new Ui::WindowBar),
      m_barBtnGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    signalsAndSlots();
    m_barBtnGroup->addButton(ui->HomeBtn, 0);
    m_barBtnGroup->addButton(ui->GalleryBtn, 1);
    m_barBtnGroup->addButton(ui->DownloadBtn, 2);
    m_barBtnGroup->addButton(ui->SettingBtn, 3);
}

WindowBar::~WindowBar()
{
    delete ui;
}

QWidget* WindowBar::GetHitWidget() const
{
    return ui->Hit;
}

void WindowBar::signalsAndSlots()
{
    connect(m_barBtnGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked), this, [this](QAbstractButton* button) {
        const int id = m_barBtnGroup->id(button);
        emit BarBtnClick(id);
    });
}
