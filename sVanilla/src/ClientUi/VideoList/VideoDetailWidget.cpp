
#include <QPushButton>

#include "Adapter/BaseVideoView.h"
#include "VideoDetailWidget.h"
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

void VideoDetailWidget::updateUi(const std::shared_ptr<Adapter::BaseVideoView>& data)
{
    ui->DetailTitle->setText(QString::fromStdString(data->Title));
    ui->DetailPusblisher->setText(QString::fromStdString(data->Publisher));
    ui->DetailDuration->setText(QString::fromStdString(data->Duration));
    ui->DetailDescription->setText(QString::fromStdString(data->Description));
}

void VideoDetailWidget::setUi()
{
    hide();
    ui->DetailPanelCloseBtn->setIcon(QIcon(":icon/video/close.svg"));
}

void VideoDetailWidget::signalsAndSlots()
{
    connect(ui->DetailPanelCloseBtn, &QPushButton::clicked, this, [this]() {
        this->hide();
    });
}
