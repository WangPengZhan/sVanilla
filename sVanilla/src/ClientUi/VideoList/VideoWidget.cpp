#include <QDir>
#include <QPushButton>
#include <QStandardPaths>
#include <QLabel>
#include <QMenu>
#include <QClipboard>

#include "Download/AbstractDownloader.h"
#include "Adapter/BilibiliVideoView.h"
#include "BiliApi/BilibiliClient.h"
#include "ThreadPool/ThreadPool.h"
#include "ThreadPool/Task.h"
#include "Util/CoverUtil.h"
#include "VideoGridWidget.h"
#include "VideoWidget.h"
#include "ui_VideoWidget.h"
#include "ClientUi/Utils/RunTask.h"
#include "Util/UrlProcess.h"
#include "ClientUi/Config//SingleConfig.h"
#include "ClientUi/Utils/SortItems.h"
#include "ClientUi/Utils/Utility.h"
#include "ClientUi/VideoList/VideoData.h"
#include "ClientUi/Storage/SearchHistoryStorage.h"
#include "ClientUi//Storage/StorageManager.h"

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoPage)
{
    ui->setupUi(this);
    signalsAndSlots();
    setUi();
}

VideoWidget::~VideoWidget()
{
    delete ui;
}

void VideoWidget::signalsAndSlots()
{
    connect(ui->btnSwitch, &Vanilla::ToggleButton::currentItemChanged, ui->videoStackedPage, &QStackedWidget::setCurrentIndex);
    connect(ui->videoStackedPage, &QStackedWidget::currentChanged, this, [this](const int index) {
        ui->videoGridInfoWidget->hide();
        ui->videoListInfoWidget->hide();
    });

    connect(ui->lineEdit, &AddLinkLineEdit::Complete, this, [this]() {
        emit parseUri(ui->lineEdit->text().toStdString());
    });

    connect(ui->lineEdit, &AddLinkLineEdit::textChanged, this, [this](const QString& text) {
        if (!text.isEmpty() && text.length() > 1)
        {
            emit updateWebsiteIcon(text.toStdString());
        }
    });

    connect(ui->btnHistory, &QPushButton::clicked, this, [this] {
        createHistoryMenu();
        const auto menuX = ui->btnHistory->width() - m_historyMenu->sizeHint().width();
        const QPoint pos = ui->btnHistory->mapToGlobal(QPoint(menuX, ui->btnHistory->sizeHint().height()));
        m_historyMenu->exec(pos);
    });
    connect(ui->btnClipboard, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        emit parseUri(clipboard->text().toStdString());
    });

    connect(ui->btnSearch, &QPushButton::clicked, this, &VideoWidget::showSearchLineEdit);
    connect(ui->lineEditSearch, &SearchLineEdit::startHide, this, &VideoWidget::hideSearchLineEdit);
    connect(ui->lineEditSearch, &SearchLineEdit::readyHide, ui->btnSearch, &QPushButton::show);
    connect(ui->lineEditSearch, &SearchLineEdit::textChanged, this, &VideoWidget::searchItem);

    connect(ui->btnReset, &QPushButton::clicked, this, &VideoWidget::resetList);

    connect(this, &VideoWidget::coverReady, ui->videoGridWidget, &VideoGridWidget::coverReady);

    connect(ui->videoGridWidget, &VideoGridWidget::infoBtnClick, this, [this](const InfoPanelData& data) {
        showInfo(ui->videoGridInfoWidget, ui->videoGrid, data.currentRow, data.previousRow);
        if (ui->videoGridInfoWidget->isVisible())
        {
            ui->videoGridInfoWidget->updateUi(data.info);
        }
    });

    connect(ui->videoGridWidget, &VideoGridWidget::downloandBtnClick, this, &VideoWidget::prepareDownloadTask);
    connect(ui->videoListWidget, &VideoListWidget::downloandBtnClick, this, &VideoWidget::prepareDownloadTask);

    connect(ui->btnDownloadSelected, &QPushButton::clicked, this, &VideoWidget::prepareDownloadTaskList);
}

void VideoWidget::setUi()
{
    ui->videoStackedPage->setCurrentWidget(ui->videoGrid);
    setNavigationBar();
    ui->btnReset->hide();
    ui->lineEditSearch->hide();
    ui->lineEditSearch->setFocusOutHide();

    ui->videoListWidget->setInfoPanelSignalPointer(ui->videoListInfoWidget, ui->videoList);
}

void VideoWidget::createHistoryMenu()
{
    if (m_historyMenu == nullptr)
    {
        m_historyMenu = new QMenu(this);
    }
    else
    {
        m_historyMenu->clear();
    }

    auto actionCallback = [this](const QString& text) {
        ui->lineEdit->setText(text);
    };
    auto historyStorage = sqlite::StorageManager::intance().searchHistoryStorage();
    auto history = historyStorage->allItems();
    util::createMenu(m_historyMenu, width() / 3, history, actionCallback);
}

void VideoWidget::showSearchLineEdit()
{
    const auto btnFilterPos = ui->btnFilter->pos();
    const auto btnSortPos = ui->btnSort->pos();
    ui->btnSearch->hide();
    ui->lineEditSearch->show();
    util::animate(ui->btnFilter, {btnFilterPos, ui->btnFilter->pos()});
    util::animate(ui->btnSort, {btnSortPos, ui->btnSort->pos()});
    ui->lineEditSearch->setFocus();
}

void VideoWidget::hideSearchLineEdit()
{
    const auto btnSearchMarginPoint = QPoint(ui->btnSearch->width(), 0);
    const auto btnSortMarginPoint = QPoint(ui->btnSort->width(), 0);
    const auto sortEndValue = ui->btnSearch->geometry().topLeft() - btnSearchMarginPoint;
    const auto filterEndValue = sortEndValue - btnSortMarginPoint;
    util::animate(ui->btnSort, {ui->btnSort->pos(), sortEndValue});
    util::animate(ui->btnFilter, {ui->btnFilter->pos(), filterEndValue});
}

void VideoWidget::setNavigationBar()
{
    const QStringList iconList({QStringLiteral(":/icon/grid.svg"), QStringLiteral(":/icon/list.svg")});
    ui->btnSwitch->setIconList(iconList);
    const QStringList textList({tr("Grid"), tr("List")});
    ui->btnSwitch->setItemList(textList);
    constexpr int columnWidth = 100;
    ui->btnSwitch->setColumnWidth(columnWidth);
    constexpr int swtchHeight = 30;
    ui->btnSwitch->setFixedHeight(swtchHeight);
}

void VideoWidget::searchItem(const QString& text)
{
    if (text.isEmpty())
    {
        return;
    }
    if (!ui->btnReset->isVisible())
    {
        showBtnReset();
    }
    const auto items = ui->videoGridWidget->getVideoInfo();
    if (m_originalList.empty())
    {
        m_originalList = items;
    }
    const std::function<std::string(std::shared_ptr<VideoInfoFull>)> getData = [](std::shared_ptr<VideoInfoFull> info) {
        return info->videoView->Title;
    };
    const auto sortedItems = sortSimilarItems(items, text.toStdString(), getData, 0.0);
    const auto& grid = ui->videoGridWidget;
    grid->clear();
    for (const auto& info : sortedItems)
    {
        addVideoItem(info);
    }
    grid->updateCovers();
}

void VideoWidget::resetList()
{
    const auto& grid = ui->videoGridWidget;
    grid->clear();
    for (const auto& item : m_originalList)
    {
        addVideoItem(item);
    }
    grid->updateCovers();
    hideBtnReset();
}

void VideoWidget::showBtnReset()
{
    ui->btnReset->show();
}

void VideoWidget::hideBtnReset()
{
    const auto btnResetWidth = ui->btnReset->sizeHint().width();
    const auto finished = [this, btnResetWidth]() {
        ui->btnReset->hide();
        ui->btnReset->setMinimumWidth(btnResetWidth);
    };
    util::animate(ui->btnReset, {btnResetWidth, 0}, "maximumWidth", finished);
}

bool VideoWidget::eventFilter(QObject* watched, QEvent* event)
{
    return QWidget::eventFilter(watched, event);
}

void VideoWidget::prepareBiliVideoView(const std::string& uri)
{
    // 1. get video view
    auto taskFunc = [this, uri]() {
        return biliapi::BilibiliClient::globalClient().getVideoView(uri);
    };
    auto callback = [this](const biliapi::VideoViewOrigin& result) {
        if (result.code != 0)
        {
            return;
        }
        clearVideo();
        prepareVideoItem(result);
    };
    runTask(taskFunc, callback, this);
}

void VideoWidget::prepareVideoItem(const biliapi::VideoViewOrigin& videoView)
{
    // after get video view:
    // 1. add 'download cover image' task
    // 2. add video item
    const QString tempPath = QApplication::applicationDirPath();  // It is now in the temporary area
    const auto views = ConvertVideoView(videoView.data);
    if (const auto playlistTitle = views.front()->PlayListTitle; !playlistTitle.empty())
    {
        const auto title = QString::fromStdString(playlistTitle) + "(" + QString::number(views.size()) + ")";
        ui->labelPlayListTitle->setText(title);
    }
    for (int i = 0; i < views.size(); ++i)
    {
        auto videoInfoFull = std::make_shared<VideoInfoFull>();
        videoInfoFull->downloadConfig = std::make_shared<DownloadConfig>(SingleConfig::instance().downloadConfig());
        videoInfoFull->videoView = views.at(i);
        // download cover
        downloadCover({views.at(i)->Cover, util::FileHelp::removeSpecialChar(videoInfoFull->getGuid()), tempPath.toStdString(), i});
        addVideoItem(videoInfoFull);
    }

    // after cover ready:
    // 1. stop spinner
    // 2. update cover
}

void VideoWidget::downloadCover(const CoverInfo& coverInfo)
{
    auto taskFunc = [coverInfo]() {
        return downloadCoverImage(coverInfo);
    };
    auto callback = [this, coverInfo](bool result) {
        if (!result)
        {
            return;
        }
        emit coverReady(coverInfo.index);
    };
    runTask(taskFunc, callback, this);
}

void VideoWidget::addVideoItem(const std::shared_ptr<VideoInfoFull>& videoInfo) const
{
    ui->videoGridWidget->addVideoItem(videoInfo);
    ui->videoListWidget->addVideoItem(videoInfo);
}

void VideoWidget::prepareDownloadTask(const std::shared_ptr<VideoInfoFull>& infoFull) const
{
    emit createBiliDownloadTask(infoFull);
}

void VideoWidget::prepareDownloadTaskList()
{
    for (const auto& item : ui->videoGridWidget->selectedItems())
    {
        auto* const itemWidget = ui->videoGridWidget->itemWidget(item);
        auto* widget = qobject_cast<VideoGridItemWidget*>(itemWidget);
        prepareDownloadTask(widget->getVideoInfo());
    }
}

void VideoWidget::clearVideo() const
{
    ui->videoGridWidget->clearVideo();
    ui->videoListWidget->clearVideo();
    ui->videoGridInfoWidget->hide();
    ui->videoListInfoWidget->hide();
}

void VideoWidget::setWebsiteIcon(const QString& iconPath)
{
    ui->lineEdit->setWebsiteIcon(iconPath);
}

void VideoWidget::setDownloadingNumber(int number) const
{
}

void VideoWidget::setDownloadedNumber(int number) const
{
}
