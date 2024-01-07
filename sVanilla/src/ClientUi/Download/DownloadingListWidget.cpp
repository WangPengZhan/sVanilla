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
    this->setObjectName(QStringLiteral("DownloadingListWidget"));
}
void DownloadingListWidget::addListItem(DownloadingItemWidget* item)
{
    this -> addItem(reinterpret_cast<QListWidgetItem*>(item));
}

DownloadingListWidget::~DownloadingListWidget()
= default;
