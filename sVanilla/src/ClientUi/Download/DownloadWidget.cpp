#include "DownloadWidget.h"
#include "ui_DownloadWidget.h"
#include "SUI/Tips/Toast.h"

DownloadWidget::DownloadWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadWidget)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, [this] {
        Toast::Show("1: looglooogloogloog success", Toast::Level::Success);
        Toast::Show("2: this is a info", Toast::Level::Info);
        Toast::Show("3: this is a warn", Toast::Level::Warn);
        Toast::Show("4: this is a error", Toast::Level::Error);
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
