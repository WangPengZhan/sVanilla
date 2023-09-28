#include "DownloadingListWidget.h"
#include "ui_DownloadingListWidget.h"

DownloadingItemWidget::DownloadingItemWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadingItemWidget)
{
    ui->setupUi(this);
}

DownloadingItemWidget::~DownloadingItemWidget()
{
    delete ui;
}

void DownloadingItemWidget::SetUi()
{
}

void DownloadingItemWidget::SignalsAndSlots()
{
}

DownloadingListWidget::DownloadingListWidget(QWidget* parent)
    : QListWidget(parent)
{
}

DownloadingListWidget::~DownloadingListWidget()
{
}
