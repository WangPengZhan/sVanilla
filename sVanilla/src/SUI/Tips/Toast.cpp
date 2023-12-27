#include "ui_Toast.h"
#include "Toast.h"

Toast::Toast(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Toast())
{
    ui->setupUi(this);
    SetUi();

    SignalsAndSlots();
}

Toast::~Toast()
{
    delete ui;
}



void Toast::SetUi()
{
}

void Toast::SignalsAndSlots()
{
}
