#include <QProcess>
#include <QDir>
#include <QPushButton>
#include <utility>

#include "UiDownloader.h"
#include "DownloadingListWidget.h"
#include "ui_DownloadingListWidget.h"

template <typename T>
QString formatSize(T bytesPerSec)
{
    constexpr double Gib = 1073741824.0;
    constexpr double Mib = 1048576.0;
    constexpr double Kib = 1024.0;
    if (bytesPerSec >= Gib)
    {
        return QString::number(bytesPerSec / Gib, 'f', 2) + "GiB";
    }
    else if (bytesPerSec >= Mib)
    {
        return QString::number(bytesPerSec / Mib, 'f', 2) + "Mib";
    }
    else if (bytesPerSec >= Kib)
    {
        return QString::number(bytesPerSec / Mib, 'f', 2) + "Mib";
    }
    else if (bytesPerSec >= Kib)
    {
        return QString::number(bytesPerSec / Kib, 'f', 2) + "Kib/s";
    }
    else
    {
        return QString::number(bytesPerSec, 'f', 3) + "B";
    }
}

template <typename T>
QString formatSpeed(T bytesPerSec)
{
    return formatSize(bytesPerSec) + "/s";
}

DownloadingItemWidget::DownloadingItemWidget(std::shared_ptr<UiDownloader> downloader, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadingItemWidget)
    , m_listWidget(nullptr)
    , m_downloader(downloader)
{
    ui->setupUi(this);
    signalsAndSlots();
    ui->labelTitle->setText(QString::fromStdString(downloader->filename()));

    ui->btnPause->setIcon(QIcon(":icon/download/start.svg"));
    ui->btnDelete->setIcon(QIcon(":icon/download/delete.svg"));
    ui->btnFolder->setIcon(QIcon(":icon/download/folder.svg"));
    ui->btnDetail->setIcon(QIcon(":icon/download/details.svg"));
    setBackgroundRole(QPalette::NoRole);
}

DownloadingItemWidget::~DownloadingItemWidget()
{
    delete ui;
}

void DownloadingItemWidget::setListWidget(QListWidget* listWidget)
{
    m_listWidget = listWidget;
}

QListWidget* DownloadingItemWidget::listWidget() const
{
    return m_listWidget;
}

std::shared_ptr<UiDownloader> DownloadingItemWidget::downloaoder()
{
    return m_downloader;
}

void DownloadingItemWidget::setStart()
{
    if (m_downloader->status() != download::AbstractDownloader::Downloading && m_downloader->status() != download::AbstractDownloader::Finished)
    {
        m_downloader->setStatus(download::AbstractDownloader::Ready);
        ui->btnPause->setChecked(false);
    }
}

void DownloadingItemWidget::setStop()
{
    if (m_downloader->status() == download::AbstractDownloader::Downloading)
    {
        m_downloader->setStatus(download::AbstractDownloader::Stopped);
        ui->btnPause->setChecked(true);
    }
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
        if (!isResume && (m_downloader->status() == download::AbstractDownloader::Paused || m_downloader->status() == download::AbstractDownloader::Waitting))
        {
            m_downloader->setStatus(download::AbstractDownloader::Resumed);
        }
        else if (isResume &&
                 (m_downloader->status() == download::AbstractDownloader::Downloading || m_downloader->status() == download::AbstractDownloader::Resumed))
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
        if (std::filesystem::u8path(m_downloader->filename()).is_relative())
        {
            filePath = QApplication::applicationDirPath() + "/" + filePath;
        }

        QStringList arguments;

#ifdef _WIN32
        QString explorerCommand = "explorer";
        arguments << "/select," << QDir::toNativeSeparators(filePath);
#elif __linux__
        QString explorerCommand = "open";
        arguments << "-R" << filePath;
#elif __APPLE__
        QString explorerCommand = "open";
        arguments << QStringLiteral("-R") << "\"" << filePath << "\"";
#endif

        QProcess::startDetached(explorerCommand, arguments);
    });

    connect(m_downloader.get(), &UiDownloader::update, this, [this](const download::DownloadInfo& info) {
        ui->labelStatus->setText(QString::fromStdString(info.stage));
        ui->labelSpeed->setText(formatSpeed(info.speed));
        if (info.total != 0)
        {
            ui->progressBar->setValue(info.complete / static_cast<double>(info.total) * 100);
        }
        if (ui->labelSize->text().isEmpty())
        {
            ui->labelSize->setText(formatSize(info.total));
        }
    });
    connect(m_downloader.get(), &UiDownloader::finished, this, [this]() {
        ui->progressBar->setValue(100);
        ui->labelStatus->setText("finished");

        // int count = m_listWidget->count();
        // for (int i = 0; i < count; ++i)
        // {
        //     if (m_listWidget->itemWidget(m_listWidget->item(i)) == this)
        //     {
        //         auto item = m_listWidget->takeItem(i);
        //         delete item;
        //         return;
        //     }
        // }
    });
}

DownloadingListWidget::DownloadingListWidget(QWidget* parent)
    : QListWidget(parent)
{
    this->setObjectName(QStringLiteral("DownloadingListWidget"));
    signalsAndSlots();
    setBackgroundRole(QPalette::NoRole);
}
void DownloadingListWidget::addDownloadItem(const std::shared_ptr<UiDownloader>& downloader)
{
    auto pWidget = new DownloadingItemWidget(downloader, this);
    pWidget->setListWidget(this);
    auto pItem = new QListWidgetItem(this);
    // pItem->setSizeHint(QSize(0, 60));
    pItem->setSizeHint(pWidget->sizeHint());
    setItemWidget(pItem, pWidget);
    m_items.insert({downloader->guid(), pItem});
}

void DownloadingListWidget::startAll()
{
    int nCount = count();
    for (int i = 0; i < nCount; ++i)
    {
        if (downloadItemWidget(i))
        {
            downloadItemWidget(i)->setStart();
        }
    }
}

void DownloadingListWidget::stopAll()
{
    int nCount = count();
    for (int i = 0; i < nCount; ++i)
    {
        if (downloadItemWidget(i))
        {
            downloadItemWidget(i)->setStop();
        }
    }
}

void DownloadingListWidget::deleteAll()
{
    m_items.clear();
    clear();
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

DownloadingItemWidget* DownloadingListWidget::downloadItemWidget(int row) const
{
    return qobject_cast<DownloadingItemWidget*>(itemWidget(item(row)));
}

void DownloadingListWidget::signalsAndSlots() const
{
}
