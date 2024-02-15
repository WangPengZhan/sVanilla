#include <QProcess>
#include <QDir>

#include <utility>

#include "UiDownloader.h"
#include "DownloadingListWidget.h"
#include "ui_DownloadingListWidget.h"
#include "Theme/StyledItemDelegate.h"

template <typename T>
QString formatSize(T bytesPerSec)
{
    const double Gib = pow(2, 30);
    const double Mib = pow(2, 20);
    const double Kib = pow(2, 10);
    if (bytesPerSec >= Gib)
    {
        return QString::number(bytesPerSec / Gib, 'g', 2) + "GiB/s";
    }
    if (bytesPerSec >= Mib)
    {
        return QString::number(bytesPerSec / Mib, 'g', 2) + "Mib/s";
    }
    if (bytesPerSec >= Kib)
    {
        return QString::number(bytesPerSec / Kib, 'g', 2) + "Kib/s";
    }
    return QString::number(bytesPerSec, 'g', 3) + "B/s";
}

DownloadingItemWidget::DownloadingItemWidget(std::shared_ptr<UiDownloader> downloader, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadingItemWidget)
    , m_downloader(downloader)
{
    ui->setupUi(this);
}

DownloadingItemWidget::~DownloadingItemWidget()
{
    delete ui;
}

void DownloadingItemWidget::setListView(QListWidget* listWidget)
{
    m_listWidget = listWidget;
}

QListWidget* DownloadingItemWidget::listWidget()
{
    return m_listWidget;
}

std::shared_ptr<UiDownloader> DownloadingItemWidget::downloaoder()
{
    return m_downloader;
}

void DownloadingItemWidget::signalsAndSlots()
{
    connect(ui->btnDelete, &QPushButton::clicked, this, [this]() {
        m_downloader->setStatus(download::AbstractDownloader::Stopped);
        if (!m_listWidget)
        {
            return;
        }

        int count = m_listWidget->count();
        for (int i = 0; i < count; ++i)
        {
            if (m_listWidget->itemWidget(m_listWidget->item(i)) == this)
            {
                auto item = m_listWidget->takeItem(i);
                delete item;
                return;
            }
        }
    });
    connect(ui->btnPause, &QPushButton::clicked, this, [this](bool isResume) {
        if (isResume && m_downloader->status() == download::AbstractDownloader::Paused)
        {
            m_downloader->setStatus(download::AbstractDownloader::Resume);

        }
        else if (m_downloader->status() == download::AbstractDownloader::Downloading || m_downloader->status() == download::AbstractDownloader::Resume)
        {
            m_downloader->setStatus(download::AbstractDownloader::Paused);
        }
        else
        {
            ui->btnPause->setChecked(!isResume);
        }
    });
    connect(ui->btnFolder, &QPushButton::clicked, this, [this]() {
        QString filePath = QString::fromStdString(m_downloader->filename());

        QProcess process;
        QStringList arguments;

        // 根据操作系统设置不同的命令和参数
#ifdef _WIN32
        QString explorerCommand = "explorer";
        arguments << "/select," << QDir::toNativeSeparators(filePath);
#elif __linux__
        QString explorerCommand = "open";
        arguments << "-R" << filePath;
#elif __APPLE__
        QString explorerCommand = "xdg-open";
        arguments << QDir::toNativeSeparators(filePath);
#endif

        process.start(explorerCommand, arguments);
    });

    connect(m_downloader.get(), &UiDownloader::update, this, [this](QString strMsg) {
        ui->labelStatus->setText(strMsg);    
    });

}


DownloadingListWidget::DownloadingListWidget(QWidget* parent)
    : QListWidget(parent)
{
    this->setObjectName(QStringLiteral("DownloadingListWidget"));
    signalsAndSlots();

}
void DownloadingListWidget::addDownloadItem(const std::shared_ptr<UiDownloader>& downloader)
{
    auto pWidget = new DownloadingItemWidget(downloader, this);
    auto pItem = new QListWidgetItem(this);
    pItem->setSizeHint(QSize(0, 60));
    setItemWidget(pItem, pWidget);
    m_items.insert({downloader->guid(), pItem});

}

void DownloadingListWidget::removeDownloadItem(const std::string& guid)
{
    if (m_items.find(guid) != m_items.end())
    {
        if (m_items[guid])
        {
            delete m_items[guid];
        }
        m_items.erase(guid);
    }
}

QListWidgetItem* DownloadingListWidget::itemFromWidget(QWidget* target)
{
    for (int i = 0; i < count(); ++i)
    {
        if (itemWidget(item(i)) == target)
        {
            return item(i);
        }
    }

    return nullptr;
}

void DownloadingListWidget::signalsAndSlots() const
{

}

