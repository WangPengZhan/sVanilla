#pragma once

#include <QListWidget>
#include <QWidget>
namespace Ui
{
class DownloadingItemWidget;
};

class DownloadingItemWidget : public QWidget
{
    Q_OBJECT

public:
    DownloadingItemWidget(QWidget* parent = nullptr);
    ~DownloadingItemWidget();

private:
    void SetUi();
    void SignalsAndSlots();

private:
    Ui::DownloadingItemWidget* ui;
};

class DownloadingListWidget : public QListWidget
{
    Q_OBJECT

public:
    DownloadingListWidget(QWidget* parent = nullptr);
    ~DownloadingListWidget();
};
