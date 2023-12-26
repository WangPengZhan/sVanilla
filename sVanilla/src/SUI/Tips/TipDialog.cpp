#include "ui_TipDialog.h"
#include "TipDialog.h"

TipDialog::TipDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::TipDialog())
{
    ui->setupUi(this);
    SetUi();

    SignalsAndSlots();
}

TipDialog::~TipDialog()
{
    delete ui;
}



void TipDialog::SetUi()
{
}

void TipDialog::SignalsAndSlots()
{
}
