#pragma once
#include <QListWidget>

#include "Aria2Net/Protocol/Protocol.h"

namespace Ui
{
class DownloadedItemWidget;
}  // namespace Ui

struct VideoInfoFull;
class DownloadedListWidget;
class DownloadedItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadedItemWidget(std::shared_ptr<VideoInfoFull> videoInfoFull, QWidget* parent = nullptr);
    ~DownloadedItemWidget();

    void setListWidget(DownloadedListWidget* listWidget);
    DownloadedListWidget* listWidget() const;
    std::shared_ptr<VideoInfoFull> videoInfoFull() const;

    void setStart();
    void setStop();

private:
    void signalsAndSlots();

private:
    Ui::DownloadedItemWidget* ui;
    DownloadedListWidget* m_listWidget;
    std::shared_ptr<VideoInfoFull> m_videoInfoFull;
};

class DownloadedListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit DownloadedListWidget(QWidget* parent = nullptr);

    void addDownloadedItem(const std::shared_ptr<VideoInfoFull>& downloader);

    void startAll();
    void stopAll();
    void deleteAll();

    void removeDownloadItem(const std::string& guid);
    QListWidgetItem* itemFromWidget(QWidget* target);
    DownloadedItemWidget* downloadItemWidget(int row) const;

private:
    void signalsAndSlots() const;

private:
    std::unordered_map<std::string, QListWidgetItem*> m_items;
};
