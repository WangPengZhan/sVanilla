#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QScrollBar>
#include <QMenu>
#include <QShortcut>
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

void VideoGridItemWidget::saveWidgetItem(QListWidgetItem* widgetItem)
{
    m_listWidgetItem = widgetItem;
}

void VideoGridItemWidget::setVideoInfo(const std::shared_ptr<VideoInfoFull>& infoFull)
{
    m_infoFull = infoFull;
    updateVideoCard();
}

std::shared_ptr<VideoInfoFull> VideoGridItemWidget::getVideoInfo()
{
    return m_infoFull;
}

void VideoGridItemWidget::setUi()
{
    const QPixmap pixmap(":/CoverSpace.svg");
    ui->cover->setPixmap(pixmap);
}

void VideoGridItemWidget::signalsAndSlots()
{
    connect(ui->btnInfo, &QPushButton::clicked, this, &VideoGridItemWidget::showInfoTigger);
    connect(ui->btnDownload, &QPushButton::clicked, this, &VideoGridItemWidget::downloadTrigger);
}

void VideoGridItemWidget::createContextMenu()
{
#ifdef _WIN32
    auto* downloadAction = new QAction(QIcon(":icon/video/download.svg"), "Download\tCTRL D", this);
    auto* infoAction = new QAction(QIcon(":icon/video/detail.svg"), "Show Infomation\tCTRL I", this);
    auto* similarAction = new QAction("Find Similar", this);
#else
    auto* downloadAction = new QAction(QIcon(":icon/video/download.svg"), "Download\t⌘ D", this);
    auto* infoAction = new QAction(QIcon(":icon/video/detail.svg"), "Show Infomation\t⌘ I", this);
    auto* similarAction = new QAction("Find Similar", this);
#endif

    connect(downloadAction, &QAction::triggered, this, &VideoGridItemWidget::downloadTrigger);
    m_menu->addAction(downloadAction);

    m_menu->addAction(infoAction);
    connect(infoAction, &QAction::triggered, this, &VideoGridItemWidget::showInfoTigger);

    m_menu->addAction(similarAction);
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
    if (m_menu == nullptr)
    {
        m_menu = new QMenu(this);
        createContextMenu();
    }
    m_menu->popup(event->globalPos());
}

VideoGridWidget::VideoGridWidget(QWidget* parent)
    : QListWidget(parent)
{
    setUi();
}

void VideoGridWidget::addVideoItem(const std::shared_ptr<VideoInfoFull>& videoView)
{
    auto* const videoItem = new VideoGridItemWidget(this);
    auto* const item = new QListWidgetItem(this);
    videoItem->saveWidgetItem(item);
    item->setSizeHint(videoItem->sizeHint());
    setItemWidget(item, videoItem);
    videoItem->setVideoInfo(videoView);
    connect(videoItem, &VideoGridItemWidget::downloadTrigger, this, [this, item]() {
        downloadItem(item);
    });
    connect(videoItem, &VideoGridItemWidget::showInfoTigger, this, [this, item]() {
        showInfo(item);
    });
}

void VideoGridWidget::clearVideo()
{
    clear();
    update();
}

void VideoGridWidget::coverReady(const int id) const
{
    auto* const itemWidget = getItem(id);
    itemWidget->updateCover();
}

void VideoGridWidget::resizeEvent(QResizeEvent* event)
{
    QListWidget::resizeEvent(event);
    adjustItemSize();
}

void VideoGridWidget::setUi()
{
    constexpr int itemSpacing = 5;
    setSpacing(itemSpacing);
    setSelectionMode(ExtendedSelection);
    constexpr int scrollStep = 5;
    verticalScrollBar()->setSingleStep(scrollStep);
    setItemShortCuts();
}

void VideoGridWidget::setItemShortCuts()
{
    auto* const downloadShortcut = new QShortcut(this);
    connect(downloadShortcut, &QShortcut::activated, this, [this]() {
        downloadItem(currentItem());
    });
    auto* const infoShortcut = new QShortcut(this);
    connect(infoShortcut, &QShortcut::activated, this, [this]() {
        showInfo(currentItem());
    });

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    downloadShortcut->setKey(QKeySequence(Qt::CTRL | Qt::Key_D));
    infoShortcut->setKey(QKeySequence(Qt::CTRL | Qt::Key_I));
#else
    downloadShortcut->setKey(QKeySequence(Qt::CTRL | Qt::Key_D));
    infoShortcut->setKey(QKeySequence(Qt::CTRL | Qt::Key_I));
#endif
}

void VideoGridWidget::adjustItemSize() const
{
    const int n = width() / itemBaseWidth;
    constexpr int itemPadding = 22;
    const int itemWidth = (width() - n * itemPadding) / n;
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

void VideoGridWidget::downloadAllItem()
{
    for (int i = 0; i < count(); ++i)
    {
        downloadItem(item(i));
    }
}

void VideoGridWidget::downloadSelectedItem()
{
    for (const auto& item : selectedItems())
    {
        downloadItem(item);
    }
}

void VideoGridWidget::downloadItem(QListWidgetItem* item)
{
    const auto gridWidget = getItem(item);
    if (gridWidget == nullptr)
    {
        return;
    }
    emit downloandBtnClick(gridWidget->getVideoInfo());
}

void VideoGridWidget::showInfo(QListWidgetItem* item)
{
    const auto gridWidget = getItem(item);
    if (gridWidget == nullptr)
    {
        return;
    }
    const auto index = row(item);
    emit infoBtnClick({previousRow, index, gridWidget->getVideoInfo()});
    previousRow = index;
}

void VideoGridWidget::updateFileName(const QString& fileName)
{
    const auto gridWidget = getItem(previousRow);
    if (gridWidget == nullptr)
    {
        return;
    }
    gridWidget->updateInfoFileName(fileName);
}

VideoGridItemWidget* VideoGridWidget::getItem(QListWidgetItem* item) const
{
    if (item == nullptr)
    {
        return nullptr;
    }
    auto* const widgetItem = itemWidget(item);
    return qobject_cast<VideoGridItemWidget*>(widgetItem);
}

VideoGridItemWidget* VideoGridWidget::getItem(const int index) const
{
    return getItem(item(index));
}

