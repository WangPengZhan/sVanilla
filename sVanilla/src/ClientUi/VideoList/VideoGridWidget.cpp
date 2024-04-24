#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QScrollBar>
#include <QMenu>
#include <QContextMenuEvent>

#include "VideoGridWidget.h"
#include "VideoDetailWidget.h"
#include "ui_VideoGridWidget.h"
#include "BiliApi/BilibiliClient.h"
#include "SUI/RoundImageWidget.h"

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
}

VideoGridItemWidget::~VideoGridItemWidget()
{
    delete ui;
}

void VideoGridItemWidget::setUi()
{
    ui->VideoGridDetailsBtn->setIcon(QIcon(":/icon/video/detail.svg"));
    ui->VideoGridDownloadBtn->setIcon(QIcon(":/icon/video/download.svg"));
    const QPixmap pixmap(":/CoverSpace.svg");
    ui->Cover->setPixmap(pixmap);
}

void VideoGridItemWidget::signalsAndSlots()
{
    connect(ui->VideoGridDetailsBtn, &QPushButton::clicked, this, &VideoGridItemWidget::detailBtnClick);
    connect(ui->VideoGridDownloadBtn, &QPushButton::clicked, this, &VideoGridItemWidget::downloadBtnClick);
}

void VideoGridItemWidget::setCover()
{
    const QString tempPath = QApplication::applicationDirPath();
    if (const QString fullPath = QDir::cleanPath(tempPath + QDir::separator() + QString::fromStdString(Identifier) + ".jpg"); QFile::exists(fullPath))
    {
        const QPixmap pixmap(fullPath);
        ui->Cover->setPixmap(pixmap);
        update();
    }
}

void VideoGridItemWidget::updateVideoCard()
{
    elideText(ui->VideoGridTitle, QString::fromStdString(m_videoView->Title));
    ui->VideoGridDuration->setText(QString::fromStdString(m_videoView->Duration));
    elideText(ui->VideoGridAuthor, QString::fromStdString(m_videoView->Publisher));
}

void VideoGridItemWidget::updateCover()
{
    setCover();
    ui->Spinner->hide();
    ui->Spinner->deleteLater();
}

QSize VideoGridItemWidget::sizeHint() const
{
    return {itemBaseWidth, itemBaseHeight};
}

void VideoGridItemWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

void VideoGridItemWidget::contextMenuEvent(QContextMenuEvent* event)
{
    auto* menu = new QMenu(this);
    auto* downloadAction = new QAction("Download", this);
    menu->addAction(downloadAction);
    auto* detailAction = new QAction("Check Detail", this);
    menu->addAction(detailAction);
    auto* similarAction = new QAction("Find Similar", this);
    menu->addAction(similarAction);
    menu->popup(event->globalPos());
}

VideoGridWidget::VideoGridWidget(QWidget* parent)
    : QListWidget(parent)
{
    setProperty("noBackground", true);
}

void VideoGridWidget::addVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    auto* const videoItem = new VideoGridItemWidget(videoView->Identifier, this);
    auto* const item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    setItemWidget(item, videoItem);
    m_items.insert(std::make_pair(videoView->Identifier, item));
    videoItem->m_videoView = videoView;
    videoItem->updateVideoCard();
    connectItemSingal(videoItem);
}

void VideoGridWidget::resizeEvent(QResizeEvent* event)
{
    adjustItemSize();
    QListWidget::resizeEvent(event);
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
    m_splitter->setSizes(QList({4, 1}));
    update();
}

void VideoGridWidget::hideDetailPanel() const
{
    m_splitter->setSizes({1, 0});
}

bool VideoGridWidget::detailPanelVisible() const
{
    return m_splitter->sizes().first() != 0;
}

VideoDetailWidget* VideoGridWidget::detailWidget() const
{
    return qobject_cast<VideoDetailWidget*>(m_splitter->widget(1));
}

void VideoGridWidget::adjustItemSize()
{
    const int n = width() / itemBaseWidth;
    const int itemWidth = (width() - 25) / n;
    const int itemHeight = static_cast<int>(static_cast<float>(itemWidth) / aspectRatio);
    setItemSize(QSize(itemWidth, itemHeight));
}

void VideoGridWidget::setItemSize(const QSize& size)
{
    for (const auto& [fst, snd] : m_items)
    {
        snd->setSizeHint(size);
    }
}

void VideoGridWidget::clearVideo()
{
    clear();
    m_items.clear();
    currentIdentifier.clear();
    update();
}

void VideoGridWidget::getSignalPointer(QSplitter* splitter)
{
    m_splitter = splitter;
}

void VideoGridWidget::coverReady(const std::string& id) const
{
    if (const auto it = m_items.find(id); it != m_items.end())
    {
        auto* const item = itemWidget(it->second);
        auto* const widget = qobject_cast<VideoGridItemWidget*>(item);
        widget->updateCover();
    }
}
