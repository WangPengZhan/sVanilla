#include "DownloadingInfoWidget.h"
#include "DownloadingListWidget.h"
#include "ui_DownloadingInfoWidget.h"
#include "Utils/SpeedUtil.h"
#include "UiDownloader.h"
#include "Adapter/BaseVideoView.h"

DownloadingInfoWidget::DownloadingInfoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadingInfoWidget)
{
    ui->setupUi(this);
    signalsAndSlots();
    hide();
}

DownloadingInfoWidget::~DownloadingInfoWidget()
{
    delete ui;
}

void DownloadingInfoWidget::updateInfoPanel(const DownloadingItemWidget* itemWidget)
{
    auto videoInfo = itemWidget->downloaoder()->videoInfoFull();
    ui->labelTitle->setText(QString::fromStdString(videoInfo->videoView->Title));
    ui->labelPublisher->setText(QString::fromStdString(videoInfo->videoView->Publisher));
    ui->labelFolderPath->setText(itemWidget->status().folderPath);
    ui->labelFileName->setText(itemWidget->status().fileName);
    ui->labelTotalSize->setText(itemWidget->status().totalSize);
    ui->labelURI->setText(itemWidget->status().uri);

    ui->labelSpeed->setText(itemWidget->status().speed);
    const auto percentage = QString::number(itemWidget->status().progress, 'f', 1);
    ui->labelDownloaded->setText(itemWidget->status().downloadedSize + "(" + percentage + "%)");
    ui->labelRemainingTime->setText(itemWidget->status().remainingTime);
}

void DownloadingInfoWidget::signalsAndSlots()
{
    connect(ui->btnClose, &QPushButton::clicked, this, &DownloadingInfoWidget::hide);
}