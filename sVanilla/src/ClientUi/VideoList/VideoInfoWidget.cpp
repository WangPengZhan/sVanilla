
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

    const std::unordered_map<std::string, std::string> rules{
        {"title",  infoFull->videoView->Title      },
        {"date",   infoFull->videoView->PublishDate},
        {"author", infoFull->videoView->Publisher  }
    };

    ui->fileNameRule->init(rules);
    ui->fileNameRule->updateLineEdit(infoFull->downloadConfig->nameRule);

    const auto isConfigDownloadDirValid = infoFull->downloadConfig->downloadDir.isEmpty();
    const auto saveFolder =
        isConfigDownloadDirValid ? QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) : infoFull->downloadConfig->downloadDir;
    ui->lineEditSaveFolder->setText(saveFolder);
}

void VideoInfoWidget::signalsAndSlots()
{
    connect(ui->btnClose, &QPushButton::clicked, this, &VideoInfoWidget::hide);
    connect(ui->fileNameRule, &NameRuleWidget::editingFinished, this, &VideoInfoWidget::fileNameEditingFinished);
}
