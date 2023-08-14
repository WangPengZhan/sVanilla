#include "VideoListWidget.h"
#include "ui_VideoListWidget.h"

VideoListItemWidget::VideoListItemWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoListItemWidget())
{
    ui->setupUi(this);
    SetUi();

    SignalsAndSlots();
}

VideoListItemWidget::~VideoListItemWidget()
{
    delete ui;
}

void VideoListItemWidget::SetUi()
{
}

void VideoListItemWidget::SignalsAndSlots()
{
}

VideoListWidget::VideoListWidget(QWidget* parent)
{
}

VideoListWidget::~VideoListWidget()
{
}
