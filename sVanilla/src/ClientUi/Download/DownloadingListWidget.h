#pragma once
#include <memory>

#include <QListWidget>
#include <QSplitter>

#include "Aria2Net/Protocol/Protocol.h"

struct DownloadingInfo;
namespace Ui
{
class DownloadingItemWidget;
}  // namespace Ui

struct VideoInfoFull;
class UiDownloader;
class DownloadingListWidget;
class DownloadingInfoWidget;

struct DonwloadingStatus
{
    QString uri;
    QString stage;
    QString fileName;
    QString folderPath;
    QString speed;
    QString totalSize;
    QString downloadedSize;
    QString remainingTime;
    double progress;
    download::AbstractDownloader::Status Status;
};

class DownloadingItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadingItemWidget(std::shared_ptr<UiDownloader> downloader, QWidget* parent = nullptr);
    ~DownloadingItemWidget();

    void setListWidget(DownloadingListWidget* listWidget, QListWidgetItem* widgetItem);
    [[nodiscard]] DownloadingListWidget* listWidget() const;
    [[nodiscard]] std::shared_ptr<UiDownloader> downloaoder() const;

    void setStart();
    void setStop();

    [[nodiscard]] DonwloadingStatus status() const;

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

    void deleteItem();
    void pauseItem(bool isResume);
    void openItemFolder();
    void updateDownloadingItem(const download::DownloadInfo& info);
    void finishedItem();

    void updateStatusIcon(download::AbstractDownloader::Status status);
    void updateItemText();
    void showInfoPanel() const;

private:
    Ui::DownloadingItemWidget* ui;
    DownloadingListWidget* m_listWidget = nullptr;
    QListWidgetItem* m_listWidgetItem = nullptr;
    std::shared_ptr<UiDownloader> m_downloader;
    DonwloadingStatus m_status;
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

    void setInfoPanelSignal(DownloadingInfoWidget* infoWidget);

    QListWidgetItem* itemFromWidget(DownloadingItemWidget* target);
    void showInfoPanel(int index);
    void hideInfoPanel() const;
    void updateInfoPanel(const DownloadingItemWidget* item) const;

signals:
    void finished(std::shared_ptr<VideoInfoFull> videoInfoFull);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    [[nodiscard]] DownloadingItemWidget* indexOfItem(int row) const;
    void setUi();
    void signalsAndSlots() const;

private:
    QSplitter* m_splitter = nullptr;
    DownloadingInfoWidget* m_infoWidget = nullptr;
    int previousRow = -1;
};
