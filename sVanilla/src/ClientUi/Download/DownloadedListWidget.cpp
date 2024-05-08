#include <QProcess>
#include <QDir>
#include <QPushButton>
#include <QMouseEvent>

#include <utility>

#include "DownloadedListWidget.h"
#include "ui_DownloadedListWidget.h"
#include "ClientUi/VideoList/VideoData.h"
#include "Adapter/BaseVideoView.h"
#include "ClientUi/Storage/FinishedItemStorage.h"
#include "ClientUi/Storage/StorageManager.h"
#include "Sqlite/SqlComposer/BaseInfo.h"
#include "Sqlite/SqlComposer/ConditionWrapper.h"
#include "ClientUi/Config/SingleConfig.h"

DownloadedItemWidget::DownloadedItemWidget(std::shared_ptr<VideoInfoFull> videoInfoFull, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadedItemWidget)
    , m_listWidget(nullptr)
    , m_videoInfoFull(std::move(videoInfoFull))
{
    ui->setupUi(this);
    signalsAndSlots();
    ui->labelTitle->setText(QString::fromStdString(m_videoInfoFull->videoView->Title));
    setBackgroundRole(QPalette::NoRole);
}

DownloadedItemWidget::~DownloadedItemWidget()
{
    delete ui;
}

void DownloadedItemWidget::setListWidget(DownloadedListWidget* listWidget)
{
    m_listWidget = listWidget;
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
}

void DownloadedItemWidget::openItemFolder()
{
    QString filePath = m_videoInfoFull->downloadConfig->downloadDir;
    if (filePath.endsWith("/") && filePath.endsWith("\\"))
    {
        filePath += m_videoInfoFull->downloadConfig->nameRule;
    }
    else
    {
        filePath += "/";
        filePath += m_videoInfoFull->downloadConfig->nameRule;
    }

    if (std::filesystem::u8path(filePath.toStdString()).is_relative())
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
    if (m_listWidget != nullptr)
    {
        m_listWidget->showInfoPanel();
    }
}

DownloadedListWidget::DownloadedListWidget(QWidget* parent)
    : QListWidget(parent)
{
    setObjectName(QStringLiteral("DownloadedListWidget"));
    signalsAndSlots();
    setBackgroundRole(QPalette::NoRole);
    m_splitter = qobject_cast<QSplitter*>(parent);
}

void DownloadedListWidget::addDownloadedItem(const std::shared_ptr<VideoInfoFull>& videoInfFull)
{
    auto* pWidget = new DownloadedItemWidget(videoInfFull, this);
    pWidget->setListWidget(this);
    auto* pItem = new QListWidgetItem(this);
    pItem->setSizeHint(pWidget->sizeHint());
    setItemWidget(pItem, pWidget);
    m_items.insert({videoInfFull->getGuid(), pItem});
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

void DownloadedListWidget::removeDownloadItem(const std::string& guid)
{
    if (m_items.find(guid) != m_items.end())
    {
        if (m_items[guid] != nullptr)
        {
            delete m_items[guid];
        }
        m_items.erase(guid);
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

void DownloadedListWidget::showInfoPanel() const
{
    if (m_splitter == nullptr)
    {
        return;
    }
    if (m_infoWidget->isHidden())
    {
        m_infoWidget->setVisible(true);
        m_infoWidget->adjustSize();
        QList<int> sizes;
        sizes.append(m_splitter->height());
        sizes.append(m_infoWidget->height());
        m_splitter->setSizes(sizes);
    }
    else
    {
        m_infoWidget->hide();
        QList<int> sizes;
        sizes.append(1);
        sizes.append(0);
        m_splitter->setSizes(sizes);
    }
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
