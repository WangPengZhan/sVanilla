
#include <QPushButton>

#include "Adapter/BaseVideoView.h"
#include "VideoInfoWidget.h"
#include "ui_VideoInfoWidget.h"
#include "ClientUi/VideoList/VideoData.h"


VideoInfoWidget::VideoInfoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoInfoWidget)
{
    ui->setupUi(this);
    hide();
    signalsAndSlots();
}

VideoInfoWidget::~VideoInfoWidget()
{
    delete ui;
}

void VideoInfoWidget::updateUi(const std::shared_ptr<VideoInfoFull>& infoFull)
{
    ui->labelTitle->setText(QString::fromStdString(infoFull->videoView->Title));
    ui->labelPublisher->setText(QString::fromStdString(infoFull->videoView->Publisher));
    ui->labelDuration->setText(QString::fromStdString(infoFull->videoView->Duration));
    ui->labelDescription->setText(QString::fromStdString(infoFull->videoView->Description));
}


void VideoInfoWidget::signalsAndSlots()
{
    connect(ui->btnClose, &QPushButton::clicked, this, &VideoInfoWidget::hide);
}
