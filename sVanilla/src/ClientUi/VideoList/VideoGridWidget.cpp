#include "VideoGridWidget.h"
#include "ui_VideoGridWidget.h"
#include "BiliApi/BilibiliClient.h"
#include "SUI/RoundImageWidget.h"

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
    connect(ui->VideoGridDetailsBtn, &QPushButton::clicked, this, &VideoGridItemWidget::detailBtnClick);
    connect(ui->VideoGridDownloadBtn, &QPushButton::clicked, [this]() {
        // emit downloadBtnClick(m_videoView);
        auto m_biliClient = BiliApi::BilibiliClient::globalClient();
        const auto playUrl = m_biliClient.GetPlayUrl(m_videoView->cid, 64, m_videoView->bvid);
        std::list<std::string> video_urls;
        std::list<std::string> audio_urls;
        if (playUrl.code != 0)
        {
            PRINTS("play url error", playUrl.message)
            PRINTS("play url error", playUrl.message)
            return;
        }

        const auto videos = playUrl.data.durl;
        PRINTS("accept_format: ", playUrl.data.accept_format)
        for (const auto& video : videos)
        {
            video_urls.push_back(video.url);
            PRINTS("video url", video.url)
        }
    });
}
void VideoGridItemWidget::setCover()
{
    QPixmap pixmap(":/CoverTest.jpeg");
    auto scaledPixmap = pixmap.scaledToWidth(this->width(), Qt::SmoothTransformation);
    ui->Cover->setFixedSize(scaledPixmap.width(), scaledPixmap.height());
    ui->Cover->setPixmap(scaledPixmap);
}

void VideoGridItemWidget::updateVideoCard()
{
    ui->VideoGridTitle->setText(QString::fromStdString(m_videoView->title));
    ui->VideoGridDuration->setText(QString::fromStdString(std::to_string(m_videoView->duration)));
    ui->VideoGridAuthor->setText(QString::fromStdString(m_videoView->owner.name));
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
void VideoGridWidget::updateVideoItem(const std::shared_ptr<BiliApi::VideoView>& videoView)
{
    const auto bvid = videoView->bvid;
    const auto item = itemWidget(m_items[bvid]);
    const auto widget = qobject_cast<VideoGridItemWidget*>(item);
    widget->m_videoView = videoView;
    widget->updateVideoCard();
}
void VideoGridWidget::setCover()
{
    auto item = this->item(0);
    auto widget = qobject_cast<VideoGridItemWidget*>(itemWidget(item));
    widget->setCover();
}
