
#include "VideoDetailWidget.h"
#include "ui_VideoDetailWidget.h"
#include "Adapter/BaseVideoView.h"

#include <QPushButton>

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

    // ui->DetailAuthor->setText(QString::fromStdString(data->Author));
}

void VideoDetailWidget::setUi()
{
    this->setMaximumWidth(150);
    ui->DetailPanelCloseBtn->setIcon(QIcon(":icon/video/close.svg"));
}

void VideoDetailWidget::signalsAndSlots()
{
    connect(ui->DetailPanelCloseBtn, &QPushButton::clicked, this, [this]() {
        this->hide();
    });
}
