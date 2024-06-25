#include <tuple>

#include <QDir>
#include <QPushButton>
#include <QStandardPaths>
#include <QActionGroup>
#include <QClipboard>
#include <QLabel>
#include <QMenu>

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
#include "ClientUi/Setting/About.h"
#include "ClientUi/Utils/MenuEventFilter.h"
#include "ClientUi/Utils/SortItems.h"
#include "ClientUi/Utils/Utility.h"
#include "ClientUi/VideoList/VideoData.h"
#include "ClientUi/Storage/SearchHistoryStorage.h"
#include "ClientUi//Storage/StorageManager.h"

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoPage)
    , m_historyMenu(new QMenu(this))
    , m_sortMenu(new QMenu(this))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
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
        showMenu(ui->btnHistory, m_historyMenu, [this]() {
            createHistoryMenu();
        });
    });
    connect(ui->btnClipboard, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        emit parseUri(clipboard->text().toStdString());
    });

    connect(ui->btnSort, &QPushButton::clicked, this, [this]() {
        showMenu(ui->btnSort, m_sortMenu);
    });

    connect(ui->btnSearch, &QPushButton::clicked, this, &VideoWidget::showSearchLineEdit);
    connect(ui->lineEditSearch, &SearchLineEdit::startHide, this, &VideoWidget::hideSearchLineEdit);
    connect(ui->lineEditSearch, &SearchLineEdit::readyHide, this, &VideoWidget::showBtnSearch);
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
    createSortMenu();
    ui->btnReset->hide();
    ui->lineEditSearch->hide();
    ui->lineEditSearch->setFocusOutHide();

    ui->videoListWidget->setInfoPanelSignalPointer(ui->videoListInfoWidget, ui->videoList);
}

void VideoWidget::createHistoryMenu()
{
    m_historyMenu->clear();

    auto actionCallback = [this](const QString& text) {
        ui->lineEdit->setText(text);
    };
    auto historyStorage = sqlite::StorageManager::intance().searchHistoryStorage();
    auto history = historyStorage->allItems();
    util::createMenu(m_historyMenu, width() / 3, history, actionCallback);
}

void VideoWidget::createSortMenu()
{
    m_sortMenu->setProperty("_vanillaStyle_Patch", "MenuWithNoBorderPatch");

    const auto sortCategoryGroup = new QActionGroup(m_sortMenu);
    sortCategoryGroup->setExclusive(true);

    const std::array sortCategory{
        m_originalOrder = new QAction(QIcon(":/icon/originalOrder.svg"), tr("Original Order")),
        m_titleOrder = new QAction(QIcon(":/icon/title.svg"), tr("Title")),
        m_dateOrder = new QAction(QIcon(":/icon/date.svg"), tr("Pushlish Date")),
        m_durationOrder = new QAction(QIcon(":/icon/duration.svg"), tr("Duration")),
    };
    for (const auto& category : sortCategory)
    {
        category->setCheckable(true);
        sortCategoryGroup->addAction(category);
    }
    m_sortMenu->addActions(sortCategoryGroup->actions());

    m_sortMenu->addSeparator();

    const auto sortByGroup = new QActionGroup(m_sortMenu);
    sortByGroup->setExclusive(true);

    const std::array sortByCategory{
        m_ascendingOrder = new QAction(tr("Ascending order")),
        m_descendingOrder = new QAction(tr("Descending order")),
    };
    for (const auto& category : sortByCategory)
    {
        category->setCheckable(true);
        sortByGroup->addAction(category);
    }
    m_sortMenu->addActions(sortByGroup->actions());

    m_ascendingOrder->setChecked(true);
    m_originalOrder->setChecked(true);

    connect(m_titleOrder, &QAction::triggered, this, [this]() {
        sortItem(OrderType::Title);
    });
    connect(m_dateOrder, &QAction::triggered, this, [this]() {
        sortItem(OrderType::Date);
    });
    connect(m_durationOrder, &QAction::triggered, this, [this]() {
        sortItem(OrderType::Duration);
    });
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

void VideoWidget::showMenu(QPushButton* btn, QMenu* menu, std::function<void()> creator)
{
    if (creator)
    {
        creator();
    }
    const auto menuX = btn->width() - menu->sizeHint().width();
    const QPoint pos = btn->mapToGlobal(QPoint(menuX, btn->sizeHint().height()));
    menu->exec(pos);
}

void VideoWidget::sortItem(OrderType orderType)
{
    const auto& grid = ui->videoGridWidget;

    auto items = grid->getVideoInfo();
    if (m_originalList.empty())
    {
        m_originalList = items;
    }
    std::function<std::string(const std::shared_ptr<VideoInfoFull>&)> getItem;
    switch (orderType)
    {
    case OrderType::Title:
    {
        getItem = [](const std::shared_ptr<VideoInfoFull>& info) {
            return info->videoView->Title;
        };
    }
    case OrderType::Date:
    {
        getItem = [](const std::shared_ptr<VideoInfoFull>& info) {
            return info->videoView->PublishDate;
        };
    }
    case OrderType::Duration:
        getItem = [](const std::shared_ptr<VideoInfoFull>& info) {
            return info->videoView->Duration;
        };
    }
    const auto sordOrder = m_ascendingOrder->isChecked() ? Qt::AscendingOrder : Qt::DescendingOrder;

    sortInitialsItems(items, getItem, sordOrder);
    m_sortedList = items;
    grid->clear();
    for (const auto& info : items)
    {
        addVideoItem(info);
    }
    grid->updateCovers();
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
    const auto& grid = ui->videoGridWidget;

    const auto items = grid->getVideoInfo();
    if (m_originalList.empty())
    {
        m_originalList = items;
    }
    const std::function getData = [](const std::shared_ptr<VideoInfoFull>& info) {
        return info->videoView->Title;
    };
    const auto sortedItems = sortSimilarItems(items, text.toStdString(), getData, 0.0);
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

void VideoWidget::showSearchLineEdit()
{
    const auto btnFilterBeforePos = ui->btnFilter->pos();
    const auto btnSortBeforePos = ui->btnSort->pos();

    ui->lineEditSearch->show();
    ui->lineEditSearch->setFocus();

    const auto btnFilterAfterPos = ui->btnFilter->pos();
    const auto btnSortAfterPos = ui->btnFilter->pos();
    util::animate(ui->btnFilter, {btnFilterBeforePos, btnFilterAfterPos});
    util::animate(ui->btnSort, {btnSortBeforePos, btnSortAfterPos});

    hideBtnSearch();
}

void VideoWidget::hideSearchLineEdit()
{
    const auto sortEndValue = ui->btnSearch->pos() - QPoint(ui->btnSearch->width(), 0);
    const auto filterEndValue = sortEndValue - QPoint(ui->btnSort->width(), 0);
    util::animate(ui->btnSort, {ui->btnSort->pos(), sortEndValue});
    util::animate(ui->btnFilter, {ui->btnFilter->pos(), filterEndValue});
}

void VideoWidget::showBtnReset()
{
    const auto maxWidth = ui->btnReset->sizeHint().width();
    util::animate(ui->btnReset, {0, maxWidth}, "maximumWidth");
    ui->btnReset->show();
}

void VideoWidget::hideBtnReset()
{
    const auto maxWidth = ui->btnReset->width();
    const auto finished = [this, maxWidth]() {
        ui->btnReset->hide();
    };
    util::animate(ui->btnReset, {maxWidth, 0}, "maximumWidth", finished);
}

void VideoWidget::showBtnSearch()
{
    const auto maxWidth = ui->btnSearch->maximumWidth();
    ui->btnSearch->show();
    util::animate(ui->btnSearch, {0, maxWidth}, "maximumWidth");
}

void VideoWidget::hideBtnSearch()
{
    const auto maxWidth = ui->btnSearch->sizeHint().width();
    const auto finished = [this, maxWidth]() {
        ui->btnSearch->hide();
        ui->btnSearch->setMaximumWidth(maxWidth);
    };
    util::animate(ui->btnSearch, {maxWidth, 0}, "maximumWidth", finished);
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
    ui->labelPlayListTitle->clear();
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
