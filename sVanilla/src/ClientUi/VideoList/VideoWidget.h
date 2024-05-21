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

    Q_SIGNAL void createBiliDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo) const;

    void clearVideo() const;
    void updateCover(int id) const;

private:
    void signalsAndSlots();
    void setUi();
    Q_SIGNAL void allReady() const;
    Q_SIGNAL void coverReady(int id) const;

private:
    Ui::VideoPage* ui;

    unsigned long totalCoverSize = 0;
    unsigned long currentCoverSize = 0;
};
