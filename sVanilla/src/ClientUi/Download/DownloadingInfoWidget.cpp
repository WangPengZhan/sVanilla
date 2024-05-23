#include "DownloadingInfoWidget.h"
#include "DownloadingListWidget.h"
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

void DownloadingInfoWidget::updateInfoPanel(const DownloadingItemWidget* itemWidget)
{
    auto setOnce = [](QLabel* label, const QString& value) {
        if (label->text().isEmpty())
        {
            label->setText(value);
        }
    };
    setOnce(ui->labelFolderPath, itemWidget->status().folderPath);
    setOnce(ui->labelFileName, itemWidget->status().fileName);
    setOnce(ui->labelTotalSize, itemWidget->status().totalSize);
    setOnce(ui->labelURI, itemWidget->status().uri);

    ui->labelSpeed->setText(itemWidget->status().speed);
    const auto percentage = QString::number(itemWidget->status().progress, 'f', 1);
    ui->labelDownloaded->setText(itemWidget->status().downloadedSize + "(" + percentage + "%)");
    ui->labelRemainingTime->setText(itemWidget->status().remainingTime);
}

void DownloadingInfoWidget::signalsAndSlots()
{
    connect(ui->btnClose, &QPushButton::clicked, this, &DownloadingInfoWidget::hide);
}