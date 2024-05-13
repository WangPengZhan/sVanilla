#include "About.h"
#include "ui_About.h"

About::About(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::About)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

About::~About()
{
    delete ui;
}

void About::setUi()
{
}

void About::signalsAndSlots()
{
}
