#include <QPushButton>
#include <QMouseEvent>

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
    }
}

void VideoListItemWidget::downloadItem() const
{
    emit m_listWidget->downloandBtnClick(m_infoFull);
}

VideoListWidget::VideoListWidget(QWidget* /*parent*/)
{
    setSelectionMode(ExtendedSelection);
}

void VideoListWidget::addVideoItem(const std::shared_ptr<VideoInfoFull>& infoFull)
{
    auto* const videoItem = new VideoListItemWidget(this);
    auto* const item = new QListWidgetItem(this);
    videoItem->setListWidget(this, item);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    videoItem->setVideoInfo(infoFull);
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
    m_infoWidget->updateUi(infoFull);
}