#pragma once
#include <QWidget>

#include "Aria2Net/Protocol/Protocol.h"
#include "Download/AbstractDownloader.h"
#include "DownloadManager.h"

namespace download
{
struct ResourseInfo;
}
QT_BEGIN_NAMESPACE
namespace Ui
{
class DownloadWidget;
}
QT_END_NAMESPACE
namespace biliapi
{
struct PlayUrlOrigin;
}
namespace download
{
class BiliDownloader;
}
struct VideoInfoFull;
class UiDownloader;
struct FinishedItem;

class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(QWidget* parent = nullptr);
    ~DownloadWidget();

    void addTaskItem(const std::list<std::string>& videos, const std::list<std::string>& audios, const std::string& fileName);
    void addDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo, download::ResourseInfo info);

    void addFinishedItem(std::shared_ptr<VideoInfoFull> videoInfo);

    void getBiliUrl(const std::shared_ptr<VideoInfoFull>& videoInfo);
    void praseBiliDownloadUrl(const biliapi::PlayUrlOrigin& playUrl, const std::shared_ptr<VideoInfoFull>& videoInfo);

    static std::shared_ptr<VideoInfoFull> finishItemToVideoInfoFull(const FinishedItem& item);

signals:
    void sigDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo, download::ResourseInfo info);
    void downloadingCountChanged(int count);
    void downloadedCountChanged(int count);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

    void addTaskItem(const std::shared_ptr<download::BiliDownloader>& biliDownloader, const std::shared_ptr<UiDownloader>& uiDownloader);

    void createSelectedActionMenu();

    void setDownloadingNumber(int number);
    void setDownloadedNumber(int number);

private:
    Ui::DownloadWidget* ui;
    DownloadManager* m_downloadManager;
};
