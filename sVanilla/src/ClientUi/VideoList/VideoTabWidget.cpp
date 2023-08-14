#include "VideoTabWidget.h"
#include "ui_VideoTabWidget.h"

VideoTabItemWidget::VideoTabItemWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoTabItemWidget)
{
    ui->setupUi(this);
}

VideoTabItemWidget::~VideoTabItemWidget()
{
    delete ui;
}

void VideoTabItemWidget::SetUi()
{
}

void VideoTabItemWidget::SignalsAndSlots()
{
}


VideoTabWidget::VideoTabWidget(QWidget *parent)
    : QListWidget(parent)
{

}

VideoTabWidget::~VideoTabWidget()
{

}
