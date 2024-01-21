#pragma once

#include <QWidget>
#include <QListWidget>
namespace Ui
{
class VideoTabItemWidget;
}

class VideoTabItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoTabItemWidget(QWidget* parent = nullptr);
    ~VideoTabItemWidget();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::VideoTabItemWidget* ui;
};

class VideoTabWidget : public QListWidget
{
    Q_OBJECT

public:
    VideoTabWidget(QWidget* parent = nullptr);
    ~VideoTabWidget();
};
