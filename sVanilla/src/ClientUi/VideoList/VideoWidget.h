#pragma once
#include <memory>
#include <string>
#include <deque>

#include <QStackedWidget>
#include <QSplitter>
#include <QTimer>

#include <BaseVideoView.h>

#include "ClientUi/Utils/SortItems.h"
#include "VideoData.h"

namespace download
{
struct ResourseInfo;
}  //  namespace download

QT_BEGIN_NAMESPACE
namespace Ui
{
class VideoPage;
}  //  namespace Ui
QT_END_NAMESPACE

struct DownloadConfig;
struct VideoInfoFull;

class QPushButton;
class DownloadTip;

class BatchAdderOnTimer
{
public:
    BatchAdderOnTimer();
    ~BatchAdderOnTimer();

    void reset();
    void addItems(const adapter::VideoView& views);

    void setAddItemFunc(std::function<void(const adapter::BaseVideoView&)> func);

private:
    void addBatch();

private:
    QTimer m_timer;
    std::deque<adapter::BaseVideoView> m_views;
    std::function<void(const adapter::BaseVideoView&)> m_addItemFunc;
};

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget();

    void prepareBiliVideoView(const std::string& uri);
    void searchedVideoItem(adapter::VideoView views);
    void addVideoItem(const std::shared_ptr<VideoInfoFull>& videoInfo) const;

    void prepareDownloadTask(const std::shared_ptr<VideoInfoFull>& infoFull) const;
    void prepareDownloadTaskList();

    void clearVideo();
    void searchUrl(const QString& url);

    void setWebsiteIcon(const QIcon& icon);
    void setDownloadingNumber(int number) const;
    void setDownloadedNumber(int number) const;
    void showHistoryList(adapter::VideoView views);

signals:
    void createDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo) const;
    void updateWebsiteIcon(const std::string& string);
    bool parseUri(const std::string& uri);

    void allReady() const;

private:
    void setUi();
    void signalsAndSlots();
    void createHistoryMenu();
    void createSortMenu();
    void setNavigationBar();
    void showMenu(QPushButton* btn, QMenu* menu, std::function<void()> creator = {});

    void sortItem(OrderType orderType);
    void searchItem(const QString& text);
    void resetList();

    void showSearchLineEdit();
    void hideSearchLineEdit();
    void showBtnReset();
    void hideBtnReset();
    void showBtnSearch();
    void hideBtnSearch();

    template <typename Widget>
    void showInfo(Widget* widget, QSplitter* splitter, int currentRow, int previousRow);

    void showViewList(const adapter::VideoView& views);

private:
    Ui::VideoPage* ui;
    QMenu* m_historyMenu{nullptr};
    QMenu* m_sortMenu{nullptr};
    QAction* m_originalOrder{nullptr};
    QAction* m_titleOrder{nullptr};
    QAction* m_dateOrder{nullptr};
    QAction* m_durationOrder{nullptr};
    QAction* m_ascendingOrder{nullptr};
    QAction* m_descendingOrder{nullptr};
    DownloadTip* m_downloadTip{nullptr};
    std::vector<std::shared_ptr<VideoInfoFull>> m_originalList;
    std::vector<std::shared_ptr<VideoInfoFull>> m_sortedList;
    BatchAdderOnTimer m_batchAdder;
};

template <typename Widget>
void VideoWidget::showInfo(Widget* widget, QSplitter* splitter, int currentRow, int previousRow)
{
    const auto setSizes = [this, widget, splitter](const bool isVisible) {
        const QList<int> sizes{splitter->height(), isVisible ? widget->height() : 0};
        splitter->setSizes(sizes);
    };

    if (widget->isVisible() && previousRow == currentRow)
    {
        widget->hide();
        setSizes(false);
    }
    else
    {
        widget->setVisible(true);
        widget->adjustSize();
        setSizes(true);
    }
}
