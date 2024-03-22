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
}

DownloadWidget::~DownloadWidget()
{
    delete ui;
}

void DownloadWidget::addTaskItem(const std::list<std::string>& videos, const std::list<std::string>& audios, const std::string& fileName)
{
    // param path: come from config file or custom setting in the future
    auto biliDownlaoder = std::make_shared<download::BiliDownloader>(videos, audios, "output", fileName);
    auto uiDownloader = std::make_shared<UiDownloader>(biliDownlaoder);
    uiDownloader->setFileName(biliDownlaoder->filename());
    ui->DownloadListWidget->addDownloadItem(uiDownloader);
    m_downloadManager->addItem(uiDownloader);
}
void DownloadWidget::addDownloadTask(const download::ResourseInfo& info)
{
    auto biliDownlaoder = std::make_shared<download::BiliDownloader>(info);
    auto uiDownloader = std::make_shared<UiDownloader>(biliDownlaoder);
    uiDownloader->setFileName(biliDownlaoder->filename());
    ui->DownloadListWidget->addDownloadItem(uiDownloader);
    m_downloadManager->addItem(uiDownloader);
}
