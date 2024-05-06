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

    void clearItem();
    void reloadItem();
    void updateStatus();

private:
    void signalsAndSlots();
    void deleteDbFinishItem();

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

    void addDownloadedItem(const std::shared_ptr<VideoInfoFull>& videoInfoFull);

    void clearAll();
    void reloadAll();
    void scan();

    void removeDownloadItem(const std::string& guid);
    QListWidgetItem* itemFromWidget(QWidget* target);
    DownloadedItemWidget* downloadItemWidget(int row) const;

signals:
    void reloadItem(std::shared_ptr<VideoInfoFull>& videoInfoFull);

private:
    void signalsAndSlots() const;

private:
    std::unordered_map<std::string, QListWidgetItem*> m_items;
};
