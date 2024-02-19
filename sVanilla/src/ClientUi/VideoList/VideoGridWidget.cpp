#include "VideoGridWidget.h"
#include "VideoDetailWidget.h"
#include "ui_VideoGridWidget.h"
#include "BiliApi/BilibiliClient.h"
#include "SUI/RoundImageWidget.h"

#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

void elideText(QLabel* label, const QString& text)
{
    if (const QFontMetrics fontMetrics(label->font()); fontMetrics.horizontalAdvance(text) > label->width())
    {
        const auto elidedText = fontMetrics.elidedText(text, Qt::ElideRight, label->width());
        label->setText(elidedText);
        label->setToolTip(text);
    }
    else
    {
        label->setText(text);
    }
}
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
    connect(ui->VideoGridDetailsBtn, &QPushButton::clicked, this, &VideoGridItemWidget::detailBtnClick);
}
QSize VideoGridItemWidget::sizeHint() const
{
    return {320,300};
}
void VideoGridItemWidget::setCover(const std::string& id)
{
    QString tempPath = QDir::tempPath();
    tempPath.append("/").append(QString::fromStdString(id)).append("jpg");
    if (QFile::exists(tempPath))
    {
        const QPixmap pixmap(tempPath);
        const auto scaledPixmap = pixmap.scaledToWidth(width(), Qt::SmoothTransformation);
        ui->Cover->setFixedSize(scaledPixmap.width(), scaledPixmap.height());
        ui->Cover->setPixmap(scaledPixmap);
    }
    else
    {
        QPixmap pixmap(":/CoverTest.jpeg");
        auto scaledPixmap = pixmap.scaledToWidth(width(), Qt::SmoothTransformation);
        ui->Cover->setFixedSize(scaledPixmap.width(), scaledPixmap.height());
        ui->Cover->setPixmap(scaledPixmap);
    }
}

void VideoGridItemWidget::updateVideoCard() const
{
    elideText(ui->VideoGridTitle, QString::fromStdString(m_videoView->Title));
    ui->VideoGridDuration->setText(QString::fromStdString(m_videoView->Duration));
    elideText(ui->VideoGridAuthor, QString::fromStdString(m_videoView->Publisher));
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

void VideoGridWidget::addVideoItem(const std::string& bvid)
{
    const auto videoItem = new VideoGridItemWidget(bvid, this);
    videoItem->setCover(bvid);
    const auto item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    m_items.insert(std::make_pair(bvid, item));
    connectItemSingal(videoItem);
}
void VideoGridWidget::updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    const auto bvid = videoView->Identifier;
    const auto item = itemWidget(m_items[bvid]);
    const auto widget = qobject_cast<VideoGridItemWidget*>(item);
    widget->m_videoView = videoView;
    widget->updateVideoCard();
}

void VideoGridWidget::connectItemSingal(const VideoGridItemWidget* itemWidget)
{
    connect(itemWidget, &VideoGridItemWidget::detailBtnClick, this, [this, itemWidget]() {
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

void VideoGridWidget::showDetailPanel()
{
    if (!detailWidget()->isVisible())
    {
        detailWidget()->show();
    }

    const auto gridItem = itemWidget(item(0));
    const auto gridItemWidth = gridItem->width();
    const auto totalWith = m_splitter->width() - detailWidget()->minimumWidth();
    const auto gridWidth = gridItemWidth * (totalWith / gridItemWidth);
    const auto detailWidth = totalWith % gridItemWidth;
    m_splitter->setSizes(QList({gridWidth, detailWidth}));
    update();
}
void VideoGridWidget::hideDetailPanel() const
{
    m_splitter->setSizes({1, 0});
}
bool VideoGridWidget::detailPanelVisible() const
{
    return m_splitter->sizes()[1] != 0;
}

VideoDetailWidget* VideoGridWidget::detailWidget() const
{
    return qobject_cast<VideoDetailWidget*>(m_splitter->widget(1));
}

void VideoGridWidget::clearVideo()
{
    this->clear();
}
void VideoGridWidget::getSignalPointer(QSplitter* splitter)
{
    m_splitter = splitter;
}
