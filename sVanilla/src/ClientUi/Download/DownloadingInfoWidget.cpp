
#include "DownloadingInfoWidget.h"
#include "ui_DownloadingInfoWidget.h"
#include "Util/SpeedUtil.h"

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

void DownloadingInfoWidget::updateInfoPanel(const DownloadingInfo& info)
{
    ui->filePath->setText(QString::fromStdString(info.filePath));
    ui->speed->setText(formatSpeed(info.downloadingInfo.speed));
    ui->totalSize->setText(formatSize(info.downloadingInfo.total));
}

void DownloadingInfoWidget::signalsAndSlots()
{
    connect(ui->closeButton, &QPushButton::clicked, this, &DownloadingInfoWidget::hide);
}