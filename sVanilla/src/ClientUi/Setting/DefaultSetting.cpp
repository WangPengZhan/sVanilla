#include "DefaultSetting.h"
#include "ui_DefaultSetting.h"
#include "ClientUi/Event.h"

DefaultSetting::DefaultSetting(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::DefaultSetting)
{
    ui->setupUi(this);
    SignalsAndSlots();
}

DefaultSetting::~DefaultSetting()
{
    delete ui;
}
void DefaultSetting::SignalsAndSlots()
{
    connect(Event::getInstance(), &Event::updateAria2Status, this, [this](std::string status) {
        ui->Aria2Status->setText(QString::fromStdString(status));
    });
    connect(Event::getInstance(), &Event::updateVersion, this, [this](std::string version) {
        ui->Aria2Version->setText(QString::fromStdString(version));
    });
    connect(Event::getInstance(), &Event::updateFeatures, this, [this](std::string features) {
        ui->Aria2Features->setText(QString::fromStdString(features));
    });
}
