#pragma once

#include <QStackedWidget>

#include "Adapter/BaseVideoView.h"

namespace download
{
struct ResourseInfo;
}
namespace biliapi
{
class PlayUrlOrigin;
class VideoViewOrigin;
}  // namespace biliapi
struct CoverInfo;
QT_BEGIN_NAMESPACE
namespace Ui
{
class VideoPage;
}
QT_END_NAMESPACE

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget();

    void loadBiliViewView(const std::string& uri);
    void prepareBiliVideoView(const std::string& uri);
    void prepareVideoItem(const std::shared_ptr<biliapi::VideoViewOrigin>& videoView);
    void downloadCover(const CoverInfo& coverInfo);
    void addVideoItem(const Adapter::BaseVideoView& videoView) const;

    void prepareDownloadTask(const std::shared_ptr<Adapter::BaseVideoView>& videoView);
    void getBiliUrl();
    void praseBiliDownloadUrl(const biliapi::PlayUrlOrigin& playUrl);
    Q_SIGNAL void createBiliDownloadTask(const download::ResourseInfo& info) const;

    void clearVideo() const;
    void updateCover(const std::string& id) const;

private:
    void signalsAndSlots();
    void setUi();
    Q_SIGNAL void allReady() const;
    Q_SIGNAL void coverReady(const std::string& id) const;

private:
    Ui::VideoPage* ui;

    unsigned long totalCoverSize = 0;
    unsigned long currentCoverSize = 0;

    std::shared_ptr<Adapter::BaseVideoView> m_currentView;
};
