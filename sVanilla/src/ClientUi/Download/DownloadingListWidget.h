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
    explicit DownloadingItemWidget(std::string gid, QWidget* parent = nullptr);
    ~DownloadingItemWidget() override;

private:
    void signalsAndSlots();
    Ui::DownloadingItemWidget* ui;
signals:
    void deleteBtnClick(std::string);

public:
    std::string m_gid;
    std::shared_ptr<aria2net::AriaTellStatus> status;
    void updateStatus();
};

class DownloadingListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit DownloadingListWidget(QWidget* parent = nullptr);
    void addTaskItem(const std::string& gid);

public slots:
    // void onCurrent(bool isCurrent);
    void updateItem(const std::shared_ptr<aria2net::AriaTellStatus>& status);

    void deleteItem(const std::string&);
    // void openInfoDialog(const std::string& gid);

private:
    void signalsAndSlots() const;
    // QTimer* downloadIntervalTimer;
    // bool isTiemrStart = false;
    std::unordered_map<std::string, QListWidgetItem*> m_items;
};
