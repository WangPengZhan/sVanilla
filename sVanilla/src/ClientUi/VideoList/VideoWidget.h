#pragma once
#include <memory>

#include <QStackedWidget>
#include <QSplitter>

#include "Adapter/BaseVideoView.h"
#include "ClientUi/Utils/SortItems.h"
#include "VideoData.h"

namespace download
{
struct ResourseInfo;
}  //  namespace download
namespace biliapi
{
class PlayUrlOrigin;
class VideoViewOrigin;
}  //  namespace biliapi
struct CoverInfo;
QT_BEGIN_NAMESPACE
namespace Ui
{
class VideoPage;
}  //  namespace Ui
QT_END_NAMESPACE
struct DownloadConfig;

struct VideoInfoFull;

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget();

    void prepareBiliVideoView(const std::string& uri);
    void prepareVideoItem(const biliapi::VideoViewOrigin& videoView);
    void downloadCover(const CoverInfo& coverInfo);
    void addVideoItem(const std::shared_ptr<VideoInfoFull>& videoInfo) const;

    void prepareDownloadTask(const std::shared_ptr<VideoInfoFull>& infoFull) const;
    void prepareDownloadTaskList();

    void clearVideo();
    void searchUrl(const QString& url);

    void setWebsiteIcon(const QString& iconPath);
    void setDownloadingNumber(int number) const;
    void setDownloadedNumber(int number) const;

signals:
    void createBiliDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo) const;
    void updateWebsiteIcon(const std::string& string);
    void parseUri(const std::string& uri);

    void allReady() const;
    void coverReady(int id) const;

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
    std::vector<std::shared_ptr<VideoInfoFull>> m_originalList;
    std::vector<std::shared_ptr<VideoInfoFull>> m_sortedList;
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
