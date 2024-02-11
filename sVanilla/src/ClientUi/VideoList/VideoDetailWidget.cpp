
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
    this->setMaximumWidth(150);
}

void VideoDetailWidget::signalsAndSlots()
{
    connect(ui->DetailPanelCloseBtn, &QPushButton::clicked,this, [this]() {
        this->hide();
    });
}
