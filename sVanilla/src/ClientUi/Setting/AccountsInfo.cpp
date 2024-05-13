#include "AccountsInfo.h"
#include "ui_AccountsInfo.h"

AccountsInfo::AccountsInfo(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AccountsInfo)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

AccountsInfo::~AccountsInfo()
{
    delete ui;
}

void AccountsInfo::setUi()
{
}

void AccountsInfo::signalsAndSlots()
{
}
