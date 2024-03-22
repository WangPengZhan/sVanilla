#include "VideoGridWidget.h"
#include "VideoDetailWidget.h"
#include "ui_VideoGridWidget.h"
#include "BiliApi/BilibiliClient.h"
#include "SUI/RoundImageWidget.h"
#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>

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

VideoGridItemWidget::VideoGridItemWidget(std::string identifier, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoGridItemWidget)
    , Identifier(std::move(identifier))
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
    connect(ui->VideoGridDownloadBtn, &QPushButton::clicked, this, &VideoGridItemWidget::downloadBtnClick);
}

QSize VideoGridItemWidget::sizeHint() const
{
    return {320, 300};
}

void VideoGridItemWidget::setCover(const std::string& id)
{
    const QString tempPath = QDir::tempPath();
    const QString fullPath = QDir::cleanPath(tempPath + QDir::separator() + QString::fromStdString(Identifier) + ".jpg");
    if (QFile::exists(fullPath))
    {
        const QPixmap pixmap(fullPath);
        const auto scaledPixmap = pixmap.scaledToWidth(width(), Qt::SmoothTransformation);
        ui->Cover->setFixedSize(scaledPixmap.width(), scaledPixmap.height());
        ui->Cover->setPixmap(scaledPixmap);
        update();
    }
    else
    {
        QPixmap pixmap(":/CoverTest.jpeg");
        auto scaledPixmap = pixmap.scaledToWidth(width(), Qt::SmoothTransformation);
        ui->Cover->setFixedSize(scaledPixmap.width(), scaledPixmap.height());
        ui->Cover->setPixmap(scaledPixmap);
    }
}

void VideoGridItemWidget::updateVideoCard()
{
    elideText(ui->VideoGridTitle, QString::fromStdString(m_videoView->Title));
    ui->VideoGridDuration->setText(QString::fromStdString(m_videoView->Duration));
    elideText(ui->VideoGridAuthor, QString::fromStdString(m_videoView->Publisher));
    setCover(Identifier);
}

VideoGridWidget::VideoGridWidget(QWidget* parent)
    : QListWidget(parent)
{
    this->setFlow(LeftToRight);
    this->setWrapping(true);
    this->setResizeMode(Adjust);
    setProperty("noBackground", true);
}

void VideoGridWidget::addVideoItem(const std::string& identifier)
{
    const auto videoItem = new VideoGridItemWidget(identifier, this);
    const auto item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    m_items.insert(std::make_pair(identifier, item));
    connectItemSingal(videoItem);
}

void VideoGridWidget::updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    const auto identifier = videoView->Identifier;
    const auto item = itemWidget(m_items[identifier]);
    const auto widget = qobject_cast<VideoGridItemWidget*>(item);
    widget->m_videoView = videoView;
    widget->updateVideoCard();
}

void VideoGridWidget::connectItemSingal(const VideoGridItemWidget* itemWidget)
{
    connect(itemWidget, &VideoGridItemWidget::detailBtnClick, this, [this, itemWidget]() {
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

    connect(itemWidget, &VideoGridItemWidget::downloadBtnClick, this, [this, itemWidget]() {
        emit downloandBtnClick(itemWidget->m_videoView);
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
    clear();
}
void VideoGridWidget::getSignalPointer(QSplitter* splitter)
{
    m_splitter = splitter;
}
