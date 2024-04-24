#include <QPushButton>
#include <QMouseEvent>

#include "VideoListWidget.h"
#include "VideoDetailWidget.h"
#include "ui_VideoListWidget.h"


VideoListItemWidget::VideoListItemWidget(std::string identifier, QWidget* parent)
    : QWidget(parent)
    , Identifier(std::move(identifier))
    , ui(new Ui::VideoListItemWidget())
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
    setSelectionMode(ExtendedSelection);
}

void VideoListWidget::addVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    auto* const videoItem = new VideoListItemWidget(videoView->Identifier, this);
    auto* const item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    m_items.insert(std::make_pair(videoView->Identifier, item));
    videoItem->m_videoView = videoView;
    videoItem->updateVideoItem();
    connectItemSingal(videoItem);
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
    clear();
}

void VideoListWidget::getSignalPointer(QSplitter* splitter)
{
    m_splitter = splitter;
}

void VideoListWidget::connectItemSingal(const VideoListItemWidget* itemWidget)
{
    connect(itemWidget, &VideoListItemWidget::detailBtnClick, this, [this, itemWidget]() {
        const auto itemIdentifier = itemWidget->Identifier;
        if (detailPanelVisible() && currentIdentifier == itemIdentifier)
        {
            hideDetailPanel();
        }
        else
        {
            showDetailPanel();
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
    m_splitter->setSizes(QList({4, 1}));
    // m_splitter->setMinimumWidth(width());

    m_splitter->update();
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
