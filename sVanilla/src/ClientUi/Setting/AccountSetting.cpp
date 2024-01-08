//
// Created by Alanus Meminius on 2024/1/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_AccountSetting.h" resolved

#include "AccountSetting.h"
#include "ui_AccountSetting.h"

AccountSetting::AccountSetting(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::AccountSetting)
{
    ui->setupUi(this);
}

AccountSetting::~AccountSetting()
{
    delete ui;
}
