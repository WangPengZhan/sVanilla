#include "VideoGridWidget.h"
#include "ui_VideoGridWidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

VideoGridItemWidget::VideoGridItemWidget(std::string bvid, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoGridItemWidget)
    , m_bvid(std::move(bvid))
{
    ui->setupUi(this);
    signalsAndSlots();
}

VideoGridItemWidget::~VideoGridItemWidget()
{
    delete ui;
}

void VideoGridItemWidget::setUi()
{
}

void VideoGridItemWidget::signalsAndSlots()
{
    connect(ui->VideoGridInformationBtn, &QPushButton::clicked, this, &VideoGridItemWidget::detailBtnClick);
}
void VideoGridItemWidget::setCover()
{
    const QPixmap pixmap(":/CoverTest.jpeg");
    const QPixmap scaledPixmap = pixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Cover->setPixmap(scaledPixmap);
}

VideoGridWidget::VideoGridWidget(QWidget* parent)
    : QListWidget(parent)
{
    this->setFrameShape(NoFrame);
    this->setFlow(LeftToRight);
    this->setWrapping(true);
    this->setResizeMode(Adjust);
}

VideoGridWidget::~VideoGridWidget() = default;

void VideoGridWidget::addVideoItem(const std::string& bvid)
{
    const auto videoItem = new VideoGridItemWidget(bvid, this);
    videoItem->setCover();
    const auto item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    m_items.insert(std::make_pair(bvid, item));
    connect(videoItem, &VideoGridItemWidget::detailBtnClick, this, &VideoGridWidget::itemDetailBtnClick);
}
void VideoGridWidget::setCover()
{
    auto item = this->item(0);
    auto widget = qobject_cast<VideoGridItemWidget*>(itemWidget(item));
    widget->setCover();
}
