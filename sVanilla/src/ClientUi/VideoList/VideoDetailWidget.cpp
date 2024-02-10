
#include "VideoDetailWidget.h"
#include <utility>
#include "ui_VideoDetailWidget.h"

VideoDetailWidget::VideoDetailWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoDetailWidget)
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

VideoDetailWidget::~VideoDetailWidget()
{
    delete ui;
}

void VideoDetailWidget::setUi()
{
}

void VideoDetailWidget::signalsAndSlots()
{
}
