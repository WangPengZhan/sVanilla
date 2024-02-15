#include "VideoListWidget.h"
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
}

void VideoListItemWidget::signalsAndSlots()
{
    // connect(ui->VideoListDetailsBtn, &QPushButton::clicked, this, &VideoListItemWidget::detailBtnClick);
}

VideoListWidget::VideoListWidget(QWidget* parent)
{
    auto* delegate = new CustomVideoListItemDelegate();
    this->setItemDelegate(delegate);
    this->setSelectionMode(ExtendedSelection);
}

VideoListWidget::~VideoListWidget() = default;

void VideoListWidget::addVideoItem(const std::string& bvid)
{
    const auto videoItem = new VideoListItemWidget(bvid, this);
    const auto item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    m_items.insert(std::make_pair(bvid, item));
    // connect(videoItem, &VideoListItemWidget::detailBtnClick, this, &VideoListWidget::itemDetailBtnClick);
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
