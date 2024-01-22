#include "VideoListWidget.h"
#include "ui_VideoListWidget.h"

VideoListItemWidget::VideoListItemWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoListItemWidget())
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

VideoListItemWidget::~VideoListItemWidget()
{
    delete ui;
}

void VideoListItemWidget::setUi()
{
}

void VideoListItemWidget::signalsAndSlots()
{
}

VideoListWidget::VideoListWidget(QWidget* parent)
{
}

VideoListWidget::~VideoListWidget() = default;
