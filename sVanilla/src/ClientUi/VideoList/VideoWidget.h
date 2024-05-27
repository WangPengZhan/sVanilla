#pragma once
#include <memory>

#include <QStackedWidget>

#include "Adapter/BaseVideoView.h"

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
    void downloadAll();
    void downloadSelected();
    void prepareDownloadTaskList();

    void clearVideo() const;
    void updateCover(int id) const;

    void setWebsiteIcon(const QString& iconPath);
    void setDownloadingNumber(int number) const;
    void setDownloadedNumber(int number) const;
    void setHistoryFunc(const std::function<const std::list<std::string>()>&);

signals:
    void createBiliDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo) const;
    void updateWebsiteIcon(const std::string& string);
    void parseUri(const std::string& uri);

private:
    void signalsAndSlots();
    void setUi();
    void createHistoryMenu();
    void showSearchLineEdit();
    void hideSearchLineEdit();
    void setNavigationBar();

signals:
    void allReady() const;
    void coverReady(int id) const;

private:
    Ui::VideoPage* ui;
    QMenu* m_historyMenu = nullptr;
    std::function<const std::list<std::string>()> getHistory;
    unsigned long totalCoverSize = 0;
    unsigned long currentCoverSize = 0;
};
