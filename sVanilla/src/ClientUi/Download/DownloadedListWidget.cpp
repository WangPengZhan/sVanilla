#include <QProcess>
#include <QDir>
#include <QPushButton>
#include <QMouseEvent>
#include <QScrollBar>

#include <utility>

#include "DownloadedListWidget.h"
#include "ui_DownloadedListWidget.h"
#include "VideoList/VideoData.h"
#include "Adapter/BaseVideoView.h"
#include "Storage/FinishedItemStorage.h"
#include "Storage/StorageManager.h"
#include "Sqlite/SqlComposer/BaseInfo.h"
#include "Sqlite/SqlComposer/ConditionWrapper.h"
#include "Config/SingleConfig.h"
#include "Utils/InfoPanelVisibleHelper.h"
#include "BaseQt/Utility.h"
#include "MainWindow/SApplication.h"
#include "Utils/SpeedUtil.h"

DownloadedItemWidget::DownloadedItemWidget(std::shared_ptr<VideoInfoFull> videoInfoFull, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadedItemWidget)
    , m_listWidget(nullptr)
    , m_videoInfoFull(std::move(videoInfoFull))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
    setBackgroundRole(QPalette::NoRole);
}

DownloadedItemWidget::~DownloadedItemWidget()
{
    delete ui;
}

void DownloadedItemWidget::setListWidget(DownloadedListWidget* listWidget, QListWidgetItem* widgetItem)
{
    m_listWidget = listWidget;
    m_listWidgetItem = widgetItem;
}

DownloadedListWidget* DownloadedItemWidget::listWidget() const
{
    return m_listWidget;
}

std::shared_ptr<VideoInfoFull> DownloadedItemWidget::videoInfoFull() const
{
    return m_videoInfoFull;
}

void DownloadedItemWidget::clearItem()
{
    ui->btnDelete->clicked();
}

void DownloadedItemWidget::reloadItem()
{
    ui->btnRestart->clicked();
}

void DownloadedItemWidget::updateStatus()
{
}

void DownloadedItemWidget::setUi()
{
    ui->labelTitle->setText(QString::fromStdString(m_videoInfoFull->videoView->Title));
    ui->labelPublisher->setText(QString::fromStdString(m_videoInfoFull->videoView->Publisher));
    ui->labelDuration->setText(QString::fromStdString(m_videoInfoFull->videoView->Duration));
    ui->labelPublishDate->setText(QString::fromStdString(m_videoInfoFull->videoView->PublishDate));
    QString fileName = QString::fromStdString(m_videoInfoFull->fileName()) + ".mp4";
    QString filePath = m_videoInfoFull->downloadConfig->downloadDir + "/" + fileName;
    QFileInfo fileInfo(filePath);
    ui->labelSize->setText(formatSize(fileInfo.size()));
}

void DownloadedItemWidget::signalsAndSlots()
{
    connect(ui->btnDelete, &QPushButton::clicked, this, &DownloadedItemWidget::deleteItem);
    connect(ui->btnRestart, &QPushButton::clicked, this, &DownloadedItemWidget::restartItem);
    connect(ui->btnFolder, &QPushButton::clicked, this, &DownloadedItemWidget::openItemFolder);
    connect(ui->btnDetail, &QPushButton::clicked, this, &DownloadedItemWidget::showInfoPanel);
}

void DownloadedItemWidget::deleteItem()
{
    deleteDbFinishItem();

    if (const auto* item = m_listWidget->itemFromWidget(this))
    {
        delete item;
        deleteLater();
        return;
    }
}

void DownloadedItemWidget::restartItem()
{
    auto& storageManager = sqlite::StorageManager::intance();
    bool isDownloaded = storageManager.isDownloaded(m_videoInfoFull->getGuid());
    if (isDownloaded)
    {
        // to do
    }

    deleteDbFinishItem();

    emit m_listWidget->reloadItem(m_videoInfoFull);

    if (const auto* item = m_listWidget->itemFromWidget(this))
    {
        delete item;
        deleteLater();
        return;
    }
    emit m_listWidget->downloadedCountChanged(m_listWidget->count());
}

void DownloadedItemWidget::openItemFolder()
{
    QString filePath = m_videoInfoFull->downloadConfig->downloadDir;
    if (filePath.endsWith("/") || filePath.endsWith("\\"))
    {
        filePath += QString::fromStdString(m_videoInfoFull->fileName());
    }
    else
    {
        filePath += "/";
        filePath += QString::fromStdString(m_videoInfoFull->fileName());
    }
    filePath += ".mp4";

    if (std::filesystem::u8path(filePath.toStdString()).is_relative())
    {
        filePath = SApplication::appDir() + "/" + filePath;
    }

    util::showInFileExplorer(filePath);
}

void DownloadedItemWidget::deleteDbFinishItem()
{
    auto& storageManager = sqlite::StorageManager::intance();
    auto& table = sqlite::TableStructInfo<FinishItemStorage::Entity>::self();
    sqlite::ConditionWrapper condition;
    condition.addCondition(table.uniqueId, sqlite::Condition::EQUALS, m_videoInfoFull->getGuid());

    storageManager.finishedItemStorage()->deleteEntities(condition);
}

void DownloadedItemWidget::showInfoPanel() const
{
    if (m_listWidget == nullptr)
    {
        return;
    }
    m_listWidget->showInfoPanel(m_videoInfoFull, m_listWidget->row(m_listWidgetItem));
}

DownloadedListWidget::DownloadedListWidget(QWidget* parent)
    : QListWidget(parent)
{
    setObjectName(QStringLiteral("DownloadedListWidget"));
    verticalScrollBar()->setSingleStep(1);
    verticalScrollBar()->setPageStep(5);
    signalsAndSlots();
    setBackgroundRole(QPalette::NoRole);
    m_splitter = qobject_cast<QSplitter*>(parent);
}

void DownloadedListWidget::addDownloadedItem(const std::shared_ptr<VideoInfoFull>& videoInfFull)
{
    auto* pWidget = new DownloadedItemWidget(videoInfFull, this);
    auto* pItem = new QListWidgetItem(this);
    pWidget->setListWidget(this, pItem);
    pItem->setSizeHint(pWidget->sizeHint());
    setItemWidget(pItem, pWidget);
    emit downloadedCountChanged(count());
}

void DownloadedListWidget::clearAll()
{
    int nCount = count();
    for (int i = 0; i < nCount; ++i)
    {
        if (indexOfItem(i) != nullptr)
        {
            indexOfItem(i)->clearItem();
        }
    }
    emit downloadedCountChanged(count());
}

void DownloadedListWidget::reloadAll()
{
    int nCount = count();
    for (int i = 0; i < nCount; ++i)
    {
        if (indexOfItem(i) != nullptr)
        {
            indexOfItem(i)->reloadItem();
        }
    }
}

void DownloadedListWidget::scan()
{
    auto& storageManager = sqlite::StorageManager::intance();
    storageManager.finishedItemStorage()->updateFileExist();

    int nCount = count();
    for (int i = 0; i < nCount; ++i)
    {
        if (indexOfItem(i) != nullptr)
        {
            indexOfItem(i)->updateStatus();
        }
    }
}

QListWidgetItem* DownloadedListWidget::itemFromWidget(QWidget* target) const
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

void DownloadedListWidget::setInfoPanelSignal(DownloadedInfoWidget* infoWidget)
{
    m_infoWidget = infoWidget;
}

void DownloadedListWidget::showInfoPanel(const std::shared_ptr<VideoInfoFull>& videoInfo, int index)
{
    setInfoPanelVisible(m_infoWidget, m_splitter, index, previousRow);
    m_infoWidget->updateUi(videoInfo);
    scrollToItem(item(index));
}

void DownloadedListWidget::hideInfoPanel() const
{
    m_infoWidget->hide();
}

DownloadedItemWidget* DownloadedListWidget::indexOfItem(int row) const
{
    return qobject_cast<DownloadedItemWidget*>(itemWidget(item(row)));
}

void DownloadedListWidget::mouseMoveEvent(QMouseEvent* event)
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

void DownloadedListWidget::signalsAndSlots() const
{
}
