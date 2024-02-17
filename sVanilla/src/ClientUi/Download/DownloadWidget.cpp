#include "DownloadWidget.h"
#include "ui_DownloadWidget.h"
#include "SUI/Tips/Toast.h"

DownloadWidget::DownloadWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadWidget)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, [this] {
        Toast::showToast("loogloogloogloogloogloogloogloogloogloog msg", Toast::Level::Success, 3000, this);
    });
    connect(ui->pushButton_2, &QPushButton::clicked, this, [this] {
        Toast::showToast("Hello World", Toast::Level::Info, 3000, this);
    });

    connect(ui->pushButton_3, &QPushButton::clicked, this, [this] {
    Toast::showToast("Hello World", Toast::Level::Error, 3000, this);
});
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
