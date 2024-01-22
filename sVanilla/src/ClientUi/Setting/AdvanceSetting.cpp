//
// Created by Alanus Meminius on 2024/1/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_AdvanceSetting.h" resolved

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
