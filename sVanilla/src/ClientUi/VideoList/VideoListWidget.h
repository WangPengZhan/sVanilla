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
    explicit VideoListItemWidget(std::string bvid, QWidget* parent = nullptr);
    ~VideoListItemWidget();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::VideoListItemWidget* ui;
    std::string m_bvid;
};

class VideoListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit VideoListWidget(QWidget* parent = nullptr);
    ~VideoListWidget();
    void addVideoItem(const std::string& bvid);
};
