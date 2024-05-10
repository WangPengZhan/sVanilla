
#include <QPushButton>

#include "Adapter/BaseVideoView.h"
#include "VideoInfoWidget.h"
#include "ui_VideoInfoWidget.h"
#include "ClientUi/Config/SingleConfig.h"
#include "ClientUi/VideoList/VideoData.h"

#include <QStandardPaths>

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
    ui->labelPublishDate->setText(QString::fromStdString(infoFull->videoView->PublishDate));
    ui->labelDescription->setText(QString::fromStdString(infoFull->videoView->Description));

    const auto fileName = infoFull->fileName.empty() ? QString::fromStdString(infoFull->videoView->Title) : QString::fromStdString(infoFull->fileName);
    ui->lineEditFile->setText(fileName);
    previousFileName = fileName;

    const auto isConfigDownloadDirValid = infoFull->downloadConfig->downloadDir.isEmpty();
    const auto saveFolder =
        isConfigDownloadDirValid ? QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) : infoFull->downloadConfig->downloadDir;
    ui->lineEditSaveFolder->setText(saveFolder);
}

void VideoInfoWidget::signalsAndSlots()
{
    connect(ui->btnClose, &QPushButton::clicked, this, &VideoInfoWidget::hide);
    connect(ui->lineEditFile, &QLineEdit::editingFinished, this, [&]() {
        previousFileName = ui->lineEditFile->text();
        emit fileNameEditingFinished(previousFileName);
    });
}
