#include <QProcess>
#include <QDir>
#include <QPushButton>
#include <QMouseEvent>
#include <QTimer>
#include <QMenu>
#include <utility>

#include "UiDownloader.h"
#include "DownloadingListWidget.h"
#include "ui_DownloadingListWidget.h"
#include "DownloadingInfoWidget.h"
#include "ClientUi/Storage/DownloadingItemStorage.h"
#include "ClientUi/Utils/InfoPanelVisibleHelper.h"
#include "ClientUi/Utils/Utility.h"
#include "Util/SpeedUtil.h"
#include "Util/TimerUtil.h"

DownloadingItemWidget::DownloadingItemWidget(std::shared_ptr<UiDownloader> downloader, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadingItemWidget)
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

DonwloadingStatus DownloadingItemWidget::status() const
{
    return m_status;
}

void DownloadingItemWidget::contextMenuEvent(QContextMenuEvent* event)
{
    auto* menu = new QMenu(this);
    auto* downloadAction = new QAction("Start Download", this);
    menu->addAction(downloadAction);
    connect(downloadAction, &QAction::triggered, this, [this]() {
        pauseItem(false);
    });

    auto* pauseAction = new QAction("Pause Download", this);
    menu->addAction(pauseAction);
    connect(pauseAction, &QAction::triggered, this, [this]() {
        pauseItem(true);
    });
    if (m_status.Status == download::AbstractDownloader::Downloading)
    {
        downloadAction->setDisabled(true);
        pauseAction->setDisabled(false);
    }
    else
    {
        downloadAction->setDisabled(false);
        pauseAction->setDisabled(true);
    }

    auto* deleteAction = new QAction("Delete", this);
    menu->addAction(deleteAction);
    connect(deleteAction, &QAction::triggered, this, &DownloadingItemWidget::deleteItem);

    menu->addSeparator();

    auto* openFolderAction = new QAction("Open Folder", this);
    menu->addAction(openFolderAction);
    connect(openFolderAction, &QAction::triggered, this, &DownloadingItemWidget::openItemFolder);

    auto* infoAction = new QAction("Show Infomation", this);
    menu->addAction(infoAction);
    connect(infoAction, &QAction::triggered, this, &DownloadingItemWidget::showInfoPanel);

    menu->popup(event->globalPos());
    QWidget::contextMenuEvent(event);
}

void DownloadingItemWidget::setUi()
{
    std::filesystem::path filepath(m_downloader->filename());
    m_status.fileName = QString::fromStdString(filepath.filename().string());
    m_status.folderPath = QString::fromStdString(filepath.parent_path().string());
    m_status.uri = QString::fromStdString(m_downloader->uri());
}

void DownloadingItemWidget::signalsAndSlots()
{
    connect(ui->btnDelete, &QPushButton::clicked, this, &DownloadingItemWidget::deleteItem);
    connect(ui->btnPause, &QPushButton::clicked, this, &DownloadingItemWidget::pauseItem);
    connect(ui->btnFolder, &QPushButton::clicked, this, &DownloadingItemWidget::openItemFolder);
    connect(ui->btnDetail, &QPushButton::clicked, this, &DownloadingItemWidget::showInfoPanel);

    connect(m_downloader.get(), &UiDownloader::finished, this, &DownloadingItemWidget::finishedItem);
    connect(m_downloader.get(), &UiDownloader::update, this, &DownloadingItemWidget::updateDownloadingItem);

    connect(m_downloader.get(), &UiDownloader::statusChanged, this, &DownloadingItemWidget::updateStatusIcon);
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

    util::showInFileExplorer(filePath);
}

void DownloadingItemWidget::updateDownloadingItem(const download::DownloadInfo& info)
{
    m_status.stage = QString::fromStdString(info.stage);
    m_status.speed = formatSpeed(info.speed);
    m_status.totalSize = formatSize(info.total);
    m_status.downloadedSize = formatSize(info.complete);
    if (info.total != 0)
    {
        const double progress = static_cast<double>(info.complete) / static_cast<double>(info.total);
        m_status.progress = progress * 100.;
        const auto remainingTime = (info.total - info.complete) / info.speed;
        m_status.remainingTime = QString::fromStdString(formatDuration(static_cast<int>(remainingTime)));
    }

    updateItemText();

    if (m_listWidget != nullptr)
    {
        emit m_listWidget->updateInfoPanel(this);
    }
}

void DownloadingItemWidget::finishedItem()
{
    constexpr auto maxProgress = 100;
    ui->progressBar->setValue(maxProgress);
    ui->labelStatus->setText("finished");

    if (const auto* item = m_listWidget->itemFromWidget(this))
    {
        emit m_listWidget->finished(m_downloader->videoInfoFull());
        delete item;
        deleteLater();
    }
}

void DownloadingItemWidget::updateStatusIcon(download::AbstractDownloader::Status status)
{
    m_status.Status = status;
    switch (status)
    {
    case download::AbstractDownloader::Downloading:
    {
        ui->btnStatus->setIcon(QIcon(QStringLiteral(":icon/download/downloading.svg")));
        break;
    }
    case download::AbstractDownloader::Paused:
    {
        ui->btnStatus->setIcon(QIcon(QStringLiteral(":icon/download/pause.svg")));
        break;
    }
    case download::AbstractDownloader::Waitting:
    {
        ui->btnStatus->setIcon(QIcon(QStringLiteral(":icon/download/waiting.svg")));
        break;
    }
    case download::AbstractDownloader::Finished:
    {
        ui->btnStatus->setIcon(QIcon(QStringLiteral(":icon/download/completed.svg")));
        break;
    }
    case download::AbstractDownloader::Error:
    {
        ui->btnStatus->setText("error");
        break;
    }
    default:
    {
        ui->btnStatus->setText("Ready");
    }
    }
}

void DownloadingItemWidget::updateItemText()
{
    ui->labelTitle->setText(m_status.fileName);
    ui->labelSpeed->setText(m_status.speed);
    ui->labelSize->setText(m_status.totalSize);
    ui->labelStatus->setText(m_status.stage);
    ui->progressBar->setValue(static_cast<int>(m_status.progress));
    ui->labelRemainingTime->setText(m_status.remainingTime);
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
    setUi();
    signalsAndSlots();
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
        if (auto* const itemClicked = this->itemAt(point); itemClicked == nullptr)
        {
            this->clearSelection();
        }
        else
        {
            itemClicked->setSelected(!itemClicked->isSelected());
            return;
        }
    }
    QListWidget::mouseMoveEvent(event);
}

DownloadingItemWidget* DownloadingListWidget::indexOfItem(int row) const
{
    return qobject_cast<DownloadingItemWidget*>(itemWidget(item(row)));
}

void DownloadingListWidget::setUi()
{
    setBackgroundRole(QPalette::NoRole);
    setSelectionMode(ExtendedSelection);
}

void DownloadingListWidget::signalsAndSlots() const
{
}

void DownloadingListWidget::showInfoPanel(int index)
{
    setInfoPanelVisible(m_infoWidget, m_splitter, index, previousRow);
    updateInfoPanel(indexOfItem(index));
}

void DownloadingListWidget::hideInfoPanel() const
{
    m_infoWidget->hide();
}

void DownloadingListWidget::updateInfoPanel(const DownloadingItemWidget* item) const
{
    if (m_infoWidget->isVisible())
    {
        m_infoWidget->updateInfoPanel(item);
    }
}
