
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
    ui->labelFilePath->setText(QString::fromStdString(info.filePath));
    ui->labelSpeed->setText(formatSpeed(info.downloadingInfo.speed));
    ui->labelTotalSize->setText(formatSize(info.downloadingInfo.total));
}

void DownloadingInfoWidget::signalsAndSlots()
{
    connect(ui->btnClose, &QPushButton::clicked, this, &DownloadingInfoWidget::hide);
}