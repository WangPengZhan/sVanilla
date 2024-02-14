#pragma once

#include "Adapter/BaseVideoView.h"

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

    void updateVideoItem();
    std::shared_ptr<Adapter::BaseVideoView> m_videoView;

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::VideoListItemWidget* ui;
    std::string Identifier;
signals:
    void detailBtnClick();
    void detailCheckBtnClick(bool isChecked);
};

class VideoListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit VideoListWidget(QWidget* parent = nullptr);
    ~VideoListWidget();
    void addVideoItem(const std::string& bvid);
    void updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView);

private:
    std::map<std::string, QListWidgetItem*> m_items;
protected:
    void mousePressEvent(QMouseEvent* event) override;
signals:
    void itemDetailBtnClick();
    void itemDetailCheckBtnClick(bool isChecked);
};
