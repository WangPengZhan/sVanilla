#include "AdvanceSettings.h"
#include "ui_AdvanceSettings.h"

AdvanceSettings::AdvanceSettings(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AdvanceSettings)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

AdvanceSettings::~AdvanceSettings()
{
    delete ui;
}

void AdvanceSettings::setUi()
{
}

void AdvanceSettings::signalsAndSlots()
{
}
