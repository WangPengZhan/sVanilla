
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

void VideoInfoWidget::setVidoInfo(std::shared_ptr<VideoInfoFull> infoFull)
{
    m_infoFull = infoFull;
    updateUi();
}

void VideoInfoWidget::updateUi()
{
    if (!m_infoFull)
    {
        return;
    }

    ui->labelTitle->setText(QString::fromStdString(m_infoFull->videoView->Title));
    ui->labelPublisher->setText(QString::fromStdString(m_infoFull->videoView->Publisher));
    ui->labelDuration->setText(QString::fromStdString(m_infoFull->videoView->Duration));
    ui->labelPublishDate->setText(QString::fromStdString(m_infoFull->videoView->PublishDate));
    ui->labelDescription->setText(QString::fromStdString(m_infoFull->videoView->Description));

    QSignalBlocker blockerNameRule(ui->fileNameRule);
    ui->fileNameRule->init(m_infoFull->nameRules());
    ui->fileNameRule->updateLineEdit(m_infoFull->downloadConfig->nameRule);

    const auto isConfigDownloadDirValid = m_infoFull->downloadConfig->downloadDir.isEmpty();
    const auto saveFolder =
        isConfigDownloadDirValid ? QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) : m_infoFull->downloadConfig->downloadDir;
    QSignalBlocker blockerSaveFolder(ui->lineEditSaveFolder);
    ui->lineEditSaveFolder->setText(saveFolder);
}

void VideoInfoWidget::signalsAndSlots()
{
    connect(ui->btnClose, &QPushButton::clicked, this, &VideoInfoWidget::hide);
    connect(ui->lineEditSaveFolder, &FileSelector::textChanged, this, [this]() {
        if (!m_infoFull)
        {
            return;
        }
        m_infoFull->downloadConfig->downloadDir = ui->lineEditSaveFolder->text();
    });
    connect(ui->fileNameRule, &NameRuleWidget::editingFinished, this, [this](const QString& newText) {
        if (!m_infoFull)
        {
            return;
        }
        m_infoFull->downloadConfig->nameRule = newText;
    });
}
