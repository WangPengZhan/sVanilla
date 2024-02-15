#include "DownloadWidget.h"
#include "ui_DownloadWidget.h"

DownloadWidget::DownloadWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadWidget)
{
    ui->setupUi(this);
}

DownloadWidget::~DownloadWidget()
{
    delete ui;
}
void DownloadWidget::addTaskItem(const std::string& gid)
{
    ui->DownloadListWidget->addTaskItem(gid);
}
void DownloadWidget::updateItem(const std::shared_ptr<aria2net::AriaTellStatus>& status)
{
    ui->DownloadListWidget->updateItem(status);
}
