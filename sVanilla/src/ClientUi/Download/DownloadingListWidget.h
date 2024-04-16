#pragma once

#include <QTimer>
#include <QListWidget>

#include "Aria2Net/Protocol/Protocol.h"

namespace Ui
{
class DownloadingItemWidget;
}

class UiDownloader;
class DownloadingItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadingItemWidget(std::shared_ptr<UiDownloader> downloader, QWidget* parent = nullptr);
    ~DownloadingItemWidget();

    void setListWidget(QListWidget* listWidget);
    QListWidget* listWidget() const;
    std::shared_ptr<UiDownloader> downloaoder();

    void setStart();
    void setStop();

private:
    void signalsAndSlots();

private:
    Ui::DownloadingItemWidget* ui;
    QListWidget* m_listWidget;
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

private:
    void signalsAndSlots() const;

private:
    std::unordered_map<std::string, QListWidgetItem*> m_items;
};
