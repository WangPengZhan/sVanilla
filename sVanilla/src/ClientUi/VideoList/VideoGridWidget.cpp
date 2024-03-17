#include "VideoGridWidget.h"
#include "ui_VideoGridWidget.h"
#include "BiliApi/BilibiliClient.h"
#include "SUI/RoundImageWidget.h"

#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

VideoGridItemWidget::VideoGridItemWidget(std::string bvid, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoGridItemWidget)
    , Identifier(std::move(bvid))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
    ui->VideoGridDetailsBtn->installEventFilter(this);
}

VideoGridItemWidget::~VideoGridItemWidget()
{
    delete ui;
}

void VideoGridItemWidget::setUi()
{
    ui->VideoGridDetailsBtn->setIcon(QIcon(":/icon/video/detail.svg"));
    ui->VideoGridDownloadBtn->setIcon(QIcon(":/icon/video/download.svg"));
}

void VideoGridItemWidget::signalsAndSlots()
{
    // connect(ui->VideoGridDownloadBtn, &QPushButton::clicked, [this]() {
    //     // emit downloadBtnClick(m_videoView);
    //     auto m_biliClient = BiliApi::BilibiliClient::globalClient();
    //     const auto playUrl = m_biliClient.GetPlayUrl(std::stoll(m_videoView->VideoId), 64, m_videoView->Identifier);
    //     std::list<std::string> video_urls;
    //     std::list<std::string> audio_urls;
    //     if (playUrl.code != 0)
    //     {
    //         PRINTS("play url error", playUrl.message)
    //         PRINTS("play url error", playUrl.message)
    //         return;
    //     }
    //
    //     const auto videos = playUrl.data.durl;
    //     PRINTS("accept_format: ", playUrl.data.accept_format)
    //     for (const auto& video : videos)
    //     {
    //         video_urls.push_back(video.url);
    //         PRINTS("video url", video.url)
    //     }
    // });
}
void VideoGridItemWidget::setCover(const std::string& id)
{
    QString tempPath = QDir::tempPath();
    tempPath.append("/").append(QString::fromStdString(id)).append("jpg");
    if (QFile::exists(tempPath))
    {
        QPixmap pixmap(tempPath);
        auto scaledPixmap = pixmap.scaledToWidth(this->width(), Qt::SmoothTransformation);
        ui->Cover->setFixedSize(scaledPixmap.width(), scaledPixmap.height());
        ui->Cover->setPixmap(scaledPixmap);
    }
    else
    {
        QPixmap pixmap(":/CoverTest.jpeg");
        auto scaledPixmap = pixmap.scaledToWidth(this->width(), Qt::SmoothTransformation);
        ui->Cover->setFixedSize(scaledPixmap.width(), scaledPixmap.height());
        ui->Cover->setPixmap(scaledPixmap);
    }
}

void VideoGridItemWidget::updateVideoCard()
{
    ui->VideoGridTitle->setText(QString::fromStdString(m_videoView->Title));
    ui->VideoGridDuration->setText(QString::fromStdString(m_videoView->Duration));
    ui->VideoGridAuthor->setText(QString::fromStdString(m_videoView->Publisher));
}

VideoGridWidget::VideoGridWidget(QWidget* parent)
    : QListWidget(parent)
{
    // this->setFrameShape(NoFrame);
    this->setFlow(LeftToRight);
    this->setWrapping(true);
    this->setResizeMode(Adjust);
    setProperty("noBackground", true);
}

VideoGridWidget::~VideoGridWidget() = default;

void VideoGridWidget::addVideoItem(const std::string& bvid)
{
    const auto videoItem = new VideoGridItemWidget(bvid, this);
    videoItem->setCover(bvid);
    const auto item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    m_items.insert(std::make_pair(bvid, item));
    // connectItemSingal(videoItem);
}
void VideoGridWidget::updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    const auto bvid = videoView->Identifier;
    const auto item = itemWidget(m_items[bvid]);
    const auto widget = qobject_cast<VideoGridItemWidget*>(item);
    widget->m_videoView = videoView;
    widget->updateVideoCard();
}
void VideoGridWidget::connectItemSingal(const VideoGridItemWidget* itemWidget) const
{
    connect(itemWidget, &VideoGridItemWidget::detailBtnClick, this, &VideoGridWidget::itemDetailBtnClick);
    connect(itemWidget,&VideoGridItemWidget::detailCheckBtnClick,this, &VideoGridWidget::handleDetialCheckBtnClick);
}
void VideoGridWidget::clearVideo()
{
  this->clear();
}
