#include "VideoListWidget.h"
#include "ui_VideoListWidget.h"
#include "Theme/StyledItemDelegate.h"

#include <QtGui/QMouseEvent>

VideoListItemWidget::VideoListItemWidget(std::string bvid, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoListItemWidget())
    , m_bvid(std::move(bvid))
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

VideoListItemWidget::~VideoListItemWidget()
{
    delete ui;
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
    Q_UNUSED(parent);
    auto* delegate = new CustomVideoListItemDelegate();
    this->setItemDelegate(delegate);
}

VideoListWidget::~VideoListWidget() = default;

void VideoListWidget::addVideoItem(const std::string& bvid)
{
    const auto videoItem = new VideoListItemWidget(bvid, this);
    const auto item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    connect(videoItem, &VideoListItemWidget::detailBtnClick, this, &VideoListWidget::itemDetailBtnClick);
}
void VideoListWidget::mousePressEvent(QMouseEvent* event)
{
    QListWidget::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        const QPoint point = event->pos();
        if (const auto item = this->itemAt(point); item == nullptr)
        {
            this->clearSelection();
        }
    }
}
