#include "ui_TipDialog.h"
#include "TipDialog.h"

TipDialog::TipDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::TipDialog())
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

TipDialog::~TipDialog()
{
    delete ui;
}

void TipDialog::setUi()
{
}

void TipDialog::signalsAndSlots()
{
}
