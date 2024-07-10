
#include "DownloadedInfoWidget.h"

#include <QFileInfo>

#include "ui_DownloadedInfoWidget.h"
#include "ClientUi/VideoList/VideoData.h"
#include "ClientUi/Config/SingleConfig.h"
#include "Adapter/BaseVideoView.h"
#include "Util/SpeedUtil.h"

DownloadedInfoWidget::DownloadedInfoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadedInfoWidget)
{
    ui->setupUi(this);
    signalsAndSlots();
    hide();
}

DownloadedInfoWidget::~DownloadedInfoWidget()
{
    delete ui;
}

void DownloadedInfoWidget::updateUi(const std::shared_ptr<VideoInfoFull>& videoInfo)
{
    ui->labelTitle->setText(QString::fromStdString(videoInfo->videoView->Title));
    ui->labelPublisher->setText(QString::fromStdString(videoInfo->videoView->Publisher));
    ui->labelDuration->setText(QString::fromStdString(videoInfo->videoView->Duration));
    ui->labelPublishDate->setText(QString::fromStdString(videoInfo->videoView->PublishDate));
    ui->labelDescription->setText(QString::fromStdString(videoInfo->videoView->Description));
    QString fileName = QString::fromStdString(videoInfo->fileName()) + ".mp4";
    ui->labelFilePath->setText(fileName);
    ui->labelFolderPath->setText(videoInfo->downloadConfig->downloadDir);
    QString filePath = videoInfo->downloadConfig->downloadDir + "/" + fileName;
    QFileInfo fileInfo(filePath);
    ui->labelTotalSize->setText(formatSize(fileInfo.size()));
}

void DownloadedInfoWidget::signalsAndSlots()
{
    connect(ui->btnClose, &QPushButton::clicked, this, &DownloadedInfoWidget::hide);
}
