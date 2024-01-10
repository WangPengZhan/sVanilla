#pragma once
#include <QTimer>
#include <QListWidget>
#include "Aria2Net/Protocol/Protocol.h"
namespace Ui
{
class DownloadingItemWidget;
}

class DownloadingItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadingItemWidget(std::string  gid, QWidget* parent = nullptr);
    ~DownloadingItemWidget() override;

private:
    void SetUi();
    void SignalsAndSlots();

private:
    Ui::DownloadingItemWidget* ui;

public:
    std::string gid;
    std::string name;
    std::string size;
    std::string speed;
    int progress = 0;
    int status = 0;
};

class DownloadingListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit DownloadingListWidget(QWidget* parent = nullptr);
    signals:
    void regularUpdate();

public slots:
    void onCurrent(bool isCurrent);
    void addTaskItem(const std::string& gid);
    void updateItem(const std::shared_ptr<aria2net::AriaTellStatus>& status);

private:
    void SignalsAndSlots() const;
    QTimer *downloadIntervalTimer;
    bool isTiemrStart = false;
    std::unordered_map<std::string, DownloadingItemWidget*> _items;
};
