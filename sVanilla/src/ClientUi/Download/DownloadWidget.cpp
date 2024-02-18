#include "DownloadWidget.h"
#include "ui_DownloadWidget.h"
#include "UiDownloader.h"
#include "Download/BiliDownloader.h"
#include "SUI/Tips/Toast.h"

DownloadWidget::DownloadWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadWidget)
    , m_downloadManager(new DownloadManager(this))
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

void DownloadWidget::addTaskItem(const std::list<std::string>& videos, const std::list<std::string>& audios, const std::string& fileName)
{
    auto biliDownlaoder = std::make_shared<download::BiliDownloader>(videos, audios, "out", fileName);
    auto uiDownloader = std::make_shared<UiDownloader>(biliDownlaoder);
    ui->DownloadListWidget->addDownloadItem(uiDownloader);
    m_downloadManager->addItem(uiDownloader);
}

