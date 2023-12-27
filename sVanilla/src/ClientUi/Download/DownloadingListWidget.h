#pragma once

#include <QListWidget>
#include <QWidget>
namespace Ui
{
class DownloadingItemWidget;
}

class DownloadingItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadingItemWidget(QWidget* parent = nullptr);
    ~DownloadingItemWidget() override;

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
    explicit DownloadingListWidget(QWidget* parent = nullptr);
    ~DownloadingListWidget() override;
};
