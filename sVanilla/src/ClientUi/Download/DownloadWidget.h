#pragma once
#include <QWidget>

#include "Aria2Net/AriaClient/AriaApi.h"
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
class FileDownloader;
}
struct VideoInfoFull;
class UiDownloader;
struct DownloadedItem;
struct DownloadingItem;

class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(QWidget* parent = nullptr);
    ~DownloadWidget();

    void addDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo);

    void addDownloadingItem(std::shared_ptr<download::FileDownloader> downloader, std::shared_ptr<VideoInfoFull> videoInfo);
    void addDownloadedItem(std::shared_ptr<VideoInfoFull> videoInfo);

    static std::shared_ptr<VideoInfoFull> downloadingItemToVideoInfoFull(const DownloadingItem& item);
    static std::shared_ptr<VideoInfoFull> finishItemToVideoInfoFull(const DownloadedItem& item);

signals:
    void sigDownloadTask(std::shared_ptr<VideoInfoFull> videoInfo);
    void downloadingCountChanged(int count);
    void downloadedCountChanged(int count);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();
    void initHistoryData();

    void addDownloadingItem(const std::shared_ptr<download::FileDownloader>& fileDownloader, const std::shared_ptr<UiDownloader>& uiDownloader);

    void createSelectedActionMenu();

    void setDownloadingNumber(int number);
    void setDownloadedNumber(int number);

private:
    Ui::DownloadWidget* ui;
    DownloadManager* m_downloadManager;
};
