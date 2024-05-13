#include <QProcess>
#include <QDir>
#include <QPushButton>
#include <QMouseEvent>
#include <QTimer>
#include <utility>

#include "UiDownloader.h"
#include "DownloadingListWidget.h"
#include "ui_DownloadingListWidget.h"
#include "DownloadingInfoWidget.h"
#include "ClientUi/Storage/DownloadingItemStorage.h"
#include "ClientUi/Utils/InfoPanelVisibleHelper.h"
#include "Util/SpeedUtil.h"

DownloadingItemWidget::DownloadingItemWidget(std::shared_ptr<UiDownloader> downloader, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadingItemWidget)
    , m_listWidget(nullptr)
    , m_downloader(std::move(downloader))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

DownloadingItemWidget::~DownloadingItemWidget()
{
    delete ui;
}

void DownloadingItemWidget::setListWidget(DownloadingListWidget* listWidget, QListWidgetItem* widgetItem)
{
    m_listWidget = listWidget;
    m_listWidgetItem = widgetItem;
}

DownloadingListWidget* DownloadingItemWidget::listWidget() const
{
    return m_listWidget;
}

std::shared_ptr<UiDownloader> DownloadingItemWidget::downloaoder() const
{
    return m_downloader;
}

void DownloadingItemWidget::setStart()
{
    const auto status = m_downloader->status();
    if (status == download::AbstractDownloader::Waitting)
    {
        m_downloader->setStatus(download::AbstractDownloader::Ready);
    }
    else if (status == download::AbstractDownloader::Paused)
    {
        m_downloader->setStatus(download::AbstractDownloader::Resumed);
        ui->btnPause->setChecked(false);
        ui->btnPause->setIcon(QIcon(":/icon/download/start.svg")); 
    }
}

void DownloadingItemWidget::setStop()
{
    if (m_downloader->status() == download::AbstractDownloader::Downloading)
    {
        m_downloader->setStatus(download::AbstractDownloader::Paused);
        ui->btnPause->setChecked(true);
        ui->btnPause->setIcon(QIcon(":/icon/download/pause.svg")); 
    }
    else if (m_downloader->status() == download::AbstractDownloader::Ready)
    {
        m_downloader->setStatus(download::AbstractDownloader::Waitting);
    }
}

void DownloadingItemWidget::setUi()
{
    ui->labelTitle->setText(QString::fromStdString(m_downloader->filename()));
}

void DownloadingItemWidget::signalsAndSlots()
{
    connect(ui->btnDelete, &QPushButton::clicked, this, &DownloadingItemWidget::deleteItem);
    connect(ui->btnPause, &QPushButton::clicked, this, &DownloadingItemWidget::pauseItem);
    connect(ui->btnFolder, &QPushButton::clicked, this, &DownloadingItemWidget::openItemFolder);
    connect(ui->btnDetail, &QPushButton::clicked, this, &DownloadingItemWidget::showInfoPanel);

    connect(m_downloader.get(), &UiDownloader::finished, this, &DownloadingItemWidget::finishedItem);
    connect(m_downloader.get(), &UiDownloader::update, this, &DownloadingItemWidget::updateDownloadingItem);
}

void DownloadingItemWidget::deleteItem()
{
    m_downloader->setStatus(download::AbstractDownloader::Stopped);
    if (m_listWidget == nullptr)
    {
        return;
    }

    if (const auto* item = m_listWidget->itemFromWidget(this))
    {
        delete item;
        deleteLater();
    }
    m_listWidget->hideInfoPanel();
}

void DownloadingItemWidget::pauseItem(bool isResume)
{
    auto status = m_downloader->status();
    auto setStatus = [&](auto newStatus) {
        if (status != newStatus)
        {
            m_downloader->setStatus(newStatus);
            status = newStatus;
        }
    };
    const bool isDownloading = status == download::AbstractDownloader::Downloading;
    const bool isPaused = (status == download::AbstractDownloader::Paused || status == download::AbstractDownloader::Pause);
    const bool isWaitting = status == download::AbstractDownloader::Waitting;
    const bool isResumed = status == download::AbstractDownloader::Resumed;
    if (!isResume && (isPaused || isWaitting))
    {
        setStatus(download::AbstractDownloader::Resumed);
    }
    else if (isResume && (isDownloading || isResumed))
    {
        setStatus(download::AbstractDownloader::Pause);
    }
    else
    {
        ui->btnPause->setChecked(!isResume);
    }
    ui->btnPause->setIcon(QIcon(isResume ? ":/icon/download/start.svg" : ":/icon/download/pause.svg"));
}

void DownloadingItemWidget::openItemFolder()
{
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
    arguments << QStringLiteral("-R") << filePath;
#endif

    QProcess::startDetached(explorerCommand, arguments);
}

void DownloadingItemWidget::updateDownloadingItem(const download::DownloadInfo& info)
{
    ui->labelStatus->setText(QString::fromStdString(info.stage));
    ui->labelSpeed->setText(formatSpeed(info.speed));
    if (info.total != 0)
    {
        const double progress = static_cast<double>(info.complete) / static_cast<double>(info.total);
        const int progressValue = static_cast<int>(progress * 100);
        ui->progressBar->setValue(progressValue);
    }
    if (ui->labelSize->text().isEmpty())
    {
        ui->labelSize->setText(formatSize(info.total));
    }

    // update info to InfoPanel
    if (m_listWidget != nullptr)
    {
        const DownloadingInfo downloadingInfo{
            .downloadingInfo = info,
            .videoInfoFull = m_downloader->videoINfoFull(),
            .filePath = m_downloader->filename(),
        };
        emit m_listWidget->updateInfoPanel(downloadingInfo);
    }
}

void DownloadingItemWidget::finishedItem()
{
    constexpr auto maxProgress = 100;
    ui->progressBar->setValue(maxProgress);
    ui->labelStatus->setText("finished");

    if (const auto* item = m_listWidget->itemFromWidget(this))
    {
        emit m_listWidget->finished(m_downloader->videoINfoFull());
        delete item;
        deleteLater();
    }
}

void DownloadingItemWidget::showInfoPanel() const
{
    if (m_listWidget == nullptr)
    {
        return;
    }
    m_listWidget->showInfoPanel(m_listWidget->row(m_listWidgetItem));
}

DownloadingListWidget::DownloadingListWidget(QWidget* parent)
    : QListWidget(parent)
{
    this->setObjectName(QStringLiteral("DownloadingListWidget"));
    signalsAndSlots();
    setBackgroundRole(QPalette::NoRole);
    m_splitter = qobject_cast<QSplitter*>(parent);
}

void DownloadingListWidget::addDownloadItem(const std::shared_ptr<UiDownloader>& downloader)
{
    auto* pWidget = new DownloadingItemWidget(downloader, this);
    auto* pItem = new QListWidgetItem(this);
    pWidget->setListWidget(this, pItem);
    pItem->setSizeHint(pWidget->sizeHint());
    setItemWidget(pItem, pWidget);
}

void DownloadingListWidget::startAll()
{
    const int nCount = count();
    for (int i = 0; i < nCount; ++i)
    {
        if (indexOfItem(i) != nullptr)
        {
            indexOfItem(i)->setStart();
        }
    }
}

void DownloadingListWidget::stopAll()
{
    const int nCount = count();
    for (int i = 0; i < nCount; ++i)
    {
        if (indexOfItem(i) != nullptr)
        {
            indexOfItem(i)->setStop();
        }
    }
}

void DownloadingListWidget::deleteAll()
{
    stopAll();
    clear();
}

void DownloadingListWidget::setInfoPanelSignal(DownloadingInfoWidget* infoWidget)
{
    m_infoWidget = infoWidget;
}

QListWidgetItem* DownloadingListWidget::itemFromWidget(DownloadingItemWidget* target)
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

void DownloadingListWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        const QPoint point = event->pos();
        if (const auto* const itemClicked = this->itemAt(point); itemClicked == nullptr)
        {
            this->clearSelection();
        }
    }
    QListWidget::mouseMoveEvent(event);
}

DownloadingItemWidget* DownloadingListWidget::indexOfItem(int row) const
{
    return qobject_cast<DownloadingItemWidget*>(itemWidget(item(row)));
}

void DownloadingListWidget::signalsAndSlots() const
{
}

void DownloadingListWidget::showInfoPanel(int index)
{
    setInfoPanelVisible(m_infoWidget, m_splitter, index, previousRow);
}

void DownloadingListWidget::hideInfoPanel() const
{
    m_infoWidget->hide();
}

void DownloadingListWidget::updateInfoPanel(const DownloadingInfo& info) const
{
    if (m_infoWidget->isVisible())
    {
        m_infoWidget->updateInfoPanel(info);
    }
}
