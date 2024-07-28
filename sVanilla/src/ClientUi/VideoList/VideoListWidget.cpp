#include <QPushButton>
#include <QMouseEvent>
#include <QMenu>
#include <QScrollBar>

#include "VideoListWidget.h"
#include "VideoInfoWidget.h"
#include "ui_VideoListWidget.h"
#include "ClientUi/Utils/InfoPanelVisibleHelper.h"
#include "ClientUi/VideoList/VideoData.h"

VideoListItemWidget::VideoListItemWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoListItemWidget())
{
    ui->setupUi(this);
    signalsAndSlots();
}

VideoListItemWidget::~VideoListItemWidget()
{
    delete ui;
}

void VideoListItemWidget::setListWidget(VideoListWidget* listWidget, QListWidgetItem* widgetItem)
{
    m_listWidget = listWidget;
    m_listWidgetItem = widgetItem;
}

void VideoListItemWidget::setVideoInfo(const std::shared_ptr<VideoInfoFull>& infoFull)
{
    m_infoFull = infoFull;
    updateVideoItem();
}

const std::shared_ptr<VideoInfoFull>& VideoListItemWidget::videoInfo() const
{
    return m_infoFull;
}

void VideoListItemWidget::updateVideoItem()
{
    ui->labelTitle->setText(QString::fromStdString(m_infoFull->videoView->Title));
    ui->labelDuration->setText(QString::fromStdString(m_infoFull->videoView->Duration));
    ui->labelAuthor->setText(QString::fromStdString(m_infoFull->videoView->Publisher));
}

void VideoListItemWidget::signalsAndSlots()
{
    connect(ui->btnInfo, &QPushButton::clicked, this, &VideoListItemWidget::showInfoPanel);
    connect(ui->btnDownload, &QPushButton::clicked, this, &VideoListItemWidget::downloadItem);
}

void VideoListItemWidget::showInfoPanel() const
{
    if (m_listWidget != nullptr)
    {
        m_listWidget->showInfoPanel(m_listWidget->row(m_listWidgetItem));
        m_listWidget->updateInfoPanel(m_infoFull);
        m_listWidget->scrollToItem(m_listWidgetItem);
    }
}

void VideoListItemWidget::createContextMenu()
{
    auto* downloadAction = new QAction(tr("Download"), this);
    m_menu->addAction(downloadAction);
    connect(downloadAction, &QAction::triggered, this, &VideoListItemWidget::downloadItem);
    auto* infoAction = new QAction(tr("Show Infomation"), this);
    m_menu->addAction(infoAction);
    connect(infoAction, &QAction::triggered, this, &VideoListItemWidget::showInfoPanel);
}

void VideoListItemWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if (m_menu == nullptr)
    {
        m_menu = new QMenu(this);
        createContextMenu();
    }
    m_menu->popup(event->globalPos());
}

void VideoListItemWidget::downloadItem() const
{
    emit m_listWidget->downloandBtnClick(m_infoFull);
}

VideoListWidget::VideoListWidget(QWidget* parent)
    : QListWidget(parent)
{
    setSelectionMode(ExtendedSelection);
}

void VideoListWidget::addVideoItem(const std::shared_ptr<VideoInfoFull>& infoFull)
{
    auto* const videoItem = new VideoListItemWidget(this);
    auto* const item = new VideoListWidgetItem(infoFull, count());
    addItem(item);
    videoItem->setListWidget(this, item);
    item->setSizeHint(videoItem->sizeHint());
    setItemWidget(item, videoItem);
    videoItem->setVideoInfo(infoFull);
}

void VideoListWidget::resizeEvent(QResizeEvent* event)
{
    QListWidget::resizeEvent(event);
    for (int i = 0; i < count(); ++i)
    {
        if (itemWidget(item(i)))
        {
            int maxWidth = width() - (verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0);
            itemWidget(item(i))->setMaximumWidth(maxWidth);
        }
    }
}

void VideoListWidget::mousePressEvent(QMouseEvent* event)
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
    QListWidget::mousePressEvent(event);
}

void VideoListWidget::clearVideo()
{
    clear();
}

void VideoListWidget::setOrderType(OrderType orderType)
{
    int num = count();
    for (int i = 0; i < num; ++i)
    {
        auto widgetItem = dynamic_cast<VideoListWidgetItem*>(item(i));
        widgetItem->setOrderType(orderType);
    }
}

void VideoListWidget::setInfoPanelSignalPointer(VideoInfoWidget* infoWidget, QSplitter* splitter)
{
    m_infoWidget = infoWidget;
    m_splitter = splitter;
}

void VideoListWidget::showInfoPanel(int row)
{
    setInfoPanelVisible(m_infoWidget, m_splitter, row, previousRow);
}

void VideoListWidget::updateInfoPanel(const std::shared_ptr<VideoInfoFull>& infoFull) const
{
    m_infoWidget->setVidoInfo(infoFull);
}

void VideoListWidget::downloadAllItem() const
{
    for (int i = 0; i < count(); ++i)
    {
        downloadItem(item(i));
    }
}

void VideoListWidget::downloadSelectedItem() const
{
    for (const auto& item : selectedItems())
    {
        downloadItem(item);
    }
}

void VideoListWidget::downloadItem(QListWidgetItem* item) const
{
    if (item == nullptr)
    {
        return;
    }
    auto* const widgetItem = itemWidget(item);
    const auto* listWidget = qobject_cast<VideoListItemWidget*>(widgetItem);
    if (listWidget == nullptr)
    {
        return;
    }
    listWidget->downloadItem();
}
