#pragma once
#include <memory>

#include <QListWidget>

#include "Aria2Net/Protocol/Protocol.h"

namespace Ui
{
class DownloadingItemWidget;
}  // namespace Ui

struct VideoInfoFull;
class UiDownloader;
class DownloadingListWidget;

class DownloadingItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadingItemWidget(std::shared_ptr<UiDownloader> downloader, QWidget* parent = nullptr);
    ~DownloadingItemWidget();

    void setListWidget(DownloadingListWidget* listWidget);
    DownloadingListWidget* listWidget() const;
    std::shared_ptr<UiDownloader> downloaoder() const;

    void setStart();
    void setStop();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::DownloadingItemWidget* ui;
    DownloadingListWidget* m_listWidget;
    std::shared_ptr<UiDownloader> m_downloader;
};

class DownloadingListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit DownloadingListWidget(QWidget* parent = nullptr);

    void addDownloadItem(const std::shared_ptr<UiDownloader>& downloader);

    void startAll();
    void stopAll();
    void deleteAll();

    void removeDownloadItem(const std::string& guid);
    QListWidgetItem* itemFromWidget(QWidget* target);
    DownloadingItemWidget* downloadItemWidget(int row) const;

signals:
    void finished(std::shared_ptr<VideoInfoFull> videoInfoFull);

private:
    void signalsAndSlots() const;

private:
    std::unordered_map<std::string, QListWidgetItem*> m_items;
};
