#pragma once

#include <QWidget>
#include <QListWidget>

namespace Ui
{
class VideoListItemWidget;
}

class VideoListItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoListItemWidget(QWidget* parent = nullptr);
    ~VideoListItemWidget();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::VideoListItemWidget* ui;
};

class VideoListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit VideoListWidget(QWidget* parent = nullptr);
    ~VideoListWidget();
};
