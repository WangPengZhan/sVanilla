#include "VideoListWidget.h"
#include "VideoDetailWidget.h"
#include "ui_VideoListWidget.h"
#include "Theme/StyledItemDelegate.h"
#include <QtGui/QMouseEvent>

VideoListItemWidget::VideoListItemWidget(std::string bvid, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoListItemWidget())
    , Identifier(std::move(bvid))
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

VideoListItemWidget::~VideoListItemWidget()
{
    delete ui;
}
void VideoListItemWidget::updateVideoItem()
{
    ui->Title->setText(QString::fromStdString(m_videoView->Title));
    ui->Duration->setText(QString::fromStdString(m_videoView->Duration));
    ui->Author->setText(QString::fromStdString(m_videoView->Publisher));
}

void VideoListItemWidget::setUi()
{
    ui->VideoListDetailsBtn->setIcon(QIcon(":/icon/video/detail.svg"));
    ui->VideoListDownloadBtn->setIcon(QIcon(":/icon/video/download.svg"));
}

void VideoListItemWidget::signalsAndSlots()
{
    connect(ui->VideoListDetailsBtn, &QPushButton::clicked, this, &VideoListItemWidget::detailBtnClick);
}

VideoListWidget::VideoListWidget(QWidget* parent)
{
    auto* delegate = new CustomVideoListItemDelegate();
    setItemDelegate(delegate);
    setSelectionMode(ExtendedSelection);
}

void VideoListWidget::addVideoItem(const std::string& bvid)
{
    const auto videoItem = new VideoListItemWidget(bvid, this);
    const auto item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    m_items.insert(std::make_pair(bvid, item));
    connectItemSingal(videoItem);
}
void VideoListWidget::updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    const auto bvid = videoView->Identifier;
    const auto item = itemWidget(m_items[bvid]);
    const auto widget = qobject_cast<VideoListItemWidget*>(item);
    widget->m_videoView = videoView;
    widget->updateVideoItem();
}
void VideoListWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        const QPoint point = event->pos();
        if (const auto itemClicked = this->itemAt(point); itemClicked == nullptr)
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
    this->clear();
}
void VideoListWidget::getSignalPointer(QSplitter* splitter)
{
    m_splitter = splitter;
}
void VideoListWidget::connectItemSingal(const VideoListItemWidget* itemWidget)
{
    connect(itemWidget, &VideoListItemWidget::detailBtnClick, this, [this, itemWidget]() {
        const auto itemIdentifier = itemWidget->Identifier;
        if (!detailPanelVisible())
        {
            showDetailPanel();
        }
        else
        {
            if (currentIdentifier == itemIdentifier)
            {
                hideDetailPanel();
            }
            else
            {
                showDetailPanel();
            }
        }
        if (currentIdentifier != itemIdentifier)
        {
            currentIdentifier = itemIdentifier;
            detailWidget()->updateUi(itemWidget->m_videoView);
        }
    });
}
void VideoListWidget::showDetailPanel()
{
    if (!detailWidget()->isVisible())
    {
        detailWidget()->show();
    }
    m_splitter->setSizes({sizeHint().height(), detailWidget()->sizeHint().height()});
}
void VideoListWidget::hideDetailPanel()
{
    m_splitter->setSizes({1, 0});
}
bool VideoListWidget::detailPanelVisible() const
{
    return m_splitter->sizes()[1] != 0;
}
VideoDetailWidget* VideoListWidget::detailWidget() const
{
    return qobject_cast<VideoDetailWidget*>(m_splitter->widget(1));
}
