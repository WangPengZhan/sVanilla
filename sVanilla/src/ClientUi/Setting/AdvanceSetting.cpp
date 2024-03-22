
#include "AdvanceSetting.h"
#include "ui_AdvanceSetting.h"

AdvanceSetting::AdvanceSetting(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AdvanceSetting)
{
    ui->setupUi(this);
}

AdvanceSetting::~AdvanceSetting()
{
    delete ui;
}
