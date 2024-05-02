
#include <QPushButton>

#include "Adapter/BaseVideoView.h"
#include "VideoDetailWidget.h"
#include "ui_VideoDetailWidget.h"
#include "ClientUi/VideoList/VideoData.h"


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

void VideoDetailWidget::updateUi(const std::shared_ptr<VideoInfoFull>& data)
{
    ui->DetailTitle->setText(QString::fromStdString(data->videoView->Title));
    ui->DetailPusblisher->setText(QString::fromStdString(data->videoView->Publisher));
    ui->DetailDuration->setText(QString::fromStdString(data->videoView->Duration));
    ui->DetailDescription->setText(QString::fromStdString(data->videoView->Description));
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
