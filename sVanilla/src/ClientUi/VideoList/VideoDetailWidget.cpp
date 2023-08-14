
#include "VideoDetailWidget.h"
#include "ui_VideoDetailWidget.h"

VideoDetailWidget::VideoDetailWidget(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VideoDetailWidget())
{
    ui->setupUi(this);
    SetUi();

    SignalsAndSlots();
}

VideoDetailWidget::~VideoDetailWidget()
{
    delete ui;
}

void VideoDetailWidget::SetUi()
{
}

void VideoDetailWidget::SignalsAndSlots()
{
}
