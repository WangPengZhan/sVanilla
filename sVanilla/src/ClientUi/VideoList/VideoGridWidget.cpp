#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QScrollBar>
#include <QMenu>
#include <QContextMenuEvent>

#include "VideoGridWidget.h"
#include "VideoInfoWidget.h"
#include "ui_VideoGridWidget.h"
#include "BiliApi/BilibiliClient.h"
#include "ClientUi/Utils/InfoPanelVisibleHelper.h"
#include "SUI/RoundImageWidget.h"
#include "ClientUi/VideoList/VideoData.h"

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

VideoGridItemWidget::VideoGridItemWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoGridItemWidget)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

VideoGridItemWidget::~VideoGridItemWidget()
{
    delete ui;
}

void VideoGridItemWidget::setGridWidget(VideoGridWidget* gridWidget, QListWidgetItem* widgetItem)
{
    m_gridWidget = gridWidget;
    m_listWidgetItem = widgetItem;
}

void VideoGridItemWidget::setVideoInfo(const std::shared_ptr<VideoInfoFull>& infoFull)
{
    m_infoFull = infoFull;
    updateVideoCard();
}

void VideoGridItemWidget::setUi()
{
    const QPixmap pixmap(":/CoverSpace.svg");
    ui->cover->setPixmap(pixmap);
}

void VideoGridItemWidget::signalsAndSlots()
{
    connect(ui->btnInfo, &QPushButton::clicked, this, &VideoGridItemWidget::showInfoPanel);
    connect(ui->btnDownload, &QPushButton::clicked, this, &VideoGridItemWidget::downloadItem);
}

void VideoGridItemWidget::showInfoPanel() const
{
    if (m_gridWidget == nullptr)
    {
        return;
    }

    m_gridWidget->showInfoPanel(m_gridWidget->row(m_listWidgetItem));
    m_gridWidget->updateInfoPanel(m_infoFull);
}

void VideoGridItemWidget::downloadItem() const
{
    emit m_gridWidget->downloandBtnClick(m_infoFull);
}

void VideoGridItemWidget::setCover()
{
    const QString tempPath = QApplication::applicationDirPath();
    const auto filePath = tempPath + QDir::separator() + QString::fromStdString(m_infoFull->getGuid()) + ".jpg";
    if (const QString fullPath = QDir::cleanPath(filePath); QFile::exists(fullPath))
    {
        const QPixmap pixmap(fullPath);
        ui->cover->setPixmap(pixmap);
        update();
    }
}

void VideoGridItemWidget::updateVideoCard()
{
    elideText(ui->labelTitle, QString::fromStdString(m_infoFull->videoView->Title));
    ui->labelDuration->setText(QString::fromStdString(m_infoFull->videoView->Duration));
    elideText(ui->labelAuthor, QString::fromStdString(m_infoFull->videoView->Publisher));
    elideText(ui->labelPublishDate, QString::fromStdString(m_infoFull->videoView->PublishDate));
}

void VideoGridItemWidget::updateCover()
{
    setCover();
    if (ui->spinner != nullptr && ui->spinner->isVisible())
    {
        ui->spinner->hide();
        ui->spinner->deleteLater();
    }
}

void VideoGridItemWidget::updateInfoFileName(const QString& fileName)
{
    m_infoFull->fileName = fileName.toStdString();
}

QSize VideoGridItemWidget::sizeHint() const
{
    return {itemBaseWidth, itemBaseHeight};
}

void VideoGridItemWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_infoFull != nullptr)
    {
        elideText(ui->labelTitle, QString::fromStdString(m_infoFull->videoView->Title));
        elideText(ui->labelAuthor, QString::fromStdString(m_infoFull->videoView->Publisher));
        elideText(ui->labelPublishDate, QString::fromStdString(m_infoFull->videoView->PublishDate));
    }
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

void VideoGridWidget::addVideoItem(const std::shared_ptr<VideoInfoFull>& videoView)
{
    auto* const videoItem = new VideoGridItemWidget(this);
    auto* const item = new QListWidgetItem(this);
    videoItem->setGridWidget(this, item);
    item->setSizeHint(videoItem->sizeHint());
    setItemWidget(item, videoItem);
    videoItem->setVideoInfo(videoView);
}

void VideoGridWidget::resizeEvent(QResizeEvent* event)
{
    adjustItemSize();
    QListWidget::resizeEvent(event);
}

void VideoGridWidget::setInfoPanelSignalPointer(VideoInfoWidget* infoWidget, QSplitter* splitter)
{
    m_infoWidget = infoWidget;
    m_splitter = splitter;
    connect(m_infoWidget, &VideoInfoWidget::fileNameEditingFinished, this, [this](const QString& fileName) {
        auto* gridWidget = itemWidget(item(previousRow));
        auto* videoItem = dynamic_cast<VideoGridItemWidget*>(gridWidget);
        videoItem->updateInfoFileName(fileName);
    });
}

void VideoGridWidget::showInfoPanel(int index)
{
    setInfoPanelVisible(m_infoWidget, m_splitter, index, previousRow);
}

void VideoGridWidget::updateInfoPanel(const std::shared_ptr<VideoInfoFull>& infoFull) const
{
    m_infoWidget->updateUi(infoFull);
}

void VideoGridWidget::adjustItemSize() const
{
    const int n = width() / itemBaseWidth;
    constexpr int itemPadding = 25;
    const int itemWidth = (width() - itemPadding) / n;
    const int itemHeight = static_cast<int>(static_cast<float>(itemWidth) / aspectRatio);
    setItemSize(QSize(itemWidth, itemHeight));
}

void VideoGridWidget::setItemSize(const QSize& size) const
{
    for (int i = 0; i < count(); ++i)
    {
        item(i)->setSizeHint(size);
    }
}

void VideoGridWidget::clearVideo()
{
    clear();
    update();
}

void VideoGridWidget::coverReady(const int id) const
{
    auto* const widget = itemWidget(item(id));
    auto* const itemWidget = qobject_cast<VideoGridItemWidget*>(widget);
    itemWidget->updateCover();
}
