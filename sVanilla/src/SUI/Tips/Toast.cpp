#include "ui_Toast.h"
#include "Toast.h"

Toast::Toast(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Toast())
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

Toast::~Toast()
{
    delete ui;
}

void Toast::setUi()
{
}

void Toast::signalsAndSlots()
{
}
