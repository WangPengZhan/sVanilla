
#include "VideoDetailWidget.h"
#include "ui_VideoDetailWidget.h"

VideoDetailWidget::VideoDetailWidget(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::VideoDetailWidget())
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
