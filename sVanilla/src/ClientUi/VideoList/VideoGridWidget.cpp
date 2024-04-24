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
}

void VideoGridItemWidget::signalsAndSlots()
{
    connect(ui->VideoGridDetailsBtn, &QPushButton::clicked, this, &VideoGridItemWidget::detailBtnClick);
    connect(ui->VideoGridDownloadBtn, &QPushButton::clicked, this, &VideoGridItemWidget::downloadBtnClick);
}

void VideoGridItemWidget::setCover()
{
    const QString tempPath = QApplication::applicationDirPath();
    const QString fullPath = QDir::cleanPath(tempPath + QDir::separator() + QString::fromStdString(Identifier) + ".jpg");
    const QPixmap pixmap(QFile::exists(fullPath) ? fullPath : ":/CoverTest.jpeg");
    ui->Cover->setPixmap(pixmap);
    update();
}

void VideoGridItemWidget::updateVideoCard()
{
    elideText(ui->VideoGridTitle, QString::fromStdString(m_videoView->Title));
    ui->VideoGridDuration->setText(QString::fromStdString(m_videoView->Duration));
    elideText(ui->VideoGridAuthor, QString::fromStdString(m_videoView->Publisher));
    setCover();
}

QSize VideoGridItemWidget::sizeHint() const
{
    return {itemBaseWidth, itemBaseHeight};
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

void VideoGridWidget::addVideoItem(const std::string& identifier)
{
    auto* const videoItem = new VideoGridItemWidget(identifier, this);
    auto* const item = new QListWidgetItem(this);
    item->setSizeHint(videoItem->sizeHint());
    this->setItemWidget(item, videoItem);
    m_items.insert(std::make_pair(identifier, item));
    connectItemSingal(videoItem);
}

void VideoGridWidget::updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    const auto identifier = videoView->Identifier;
    auto* const item = itemWidget(m_items[identifier]);
    auto* const widget = qobject_cast<VideoGridItemWidget*>(item);
    widget->m_videoView = videoView;
    widget->updateVideoCard();
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
