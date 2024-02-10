#include "VideoListWidget.h"
#include "ui_VideoListWidget.h"

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
}

VideoListWidget::VideoListWidget(QWidget* parent)
{
}

VideoListWidget::~VideoListWidget() = default;

void VideoListWidget::addVideoItem(const std::string& bvid)
{
    const auto videoItem = new VideoListItemWidget(bvid, this);
    const auto item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
}
