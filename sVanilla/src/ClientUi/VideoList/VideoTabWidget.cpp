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

void VideoTabItemWidget::setUi()
{
}

void VideoTabItemWidget::signalsAndSlots()
{
}

VideoTabWidget::VideoTabWidget(QWidget* parent)
    : QListWidget(parent)
{
}

VideoTabWidget::~VideoTabWidget() = default;
