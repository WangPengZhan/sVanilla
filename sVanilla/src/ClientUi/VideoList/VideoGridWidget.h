#pragma once

#include "Adapter/VideoView.h"
#include "BiliApi/BiliApi.h"

#include <QWidget>
#include <QListWidget>
namespace Ui
{
class VideoGridItemWidget;
}

class VideoGridItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoGridItemWidget(std::string bvid, QWidget* parent = nullptr);
    ~VideoGridItemWidget();
    void setCover(const std::string& id);
    void updateVideoCard();
    std::shared_ptr<Adapter::VideoView> m_videoView;

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::VideoGridItemWidget* ui;
public:
    std::string Identifier;
signals:
    void detailBtnClick();
    void detailCheckBtnClick(bool isChecked);
};

class VideoGridWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit VideoGridWidget(QWidget* parent = nullptr);
    ~VideoGridWidget();

    void addVideoItem(const std::string& bvid);
    void updateVideoItem(const std::shared_ptr<Adapter::VideoView>& videoView);

private:
    std::map<std::string, QListWidgetItem*> m_items;
    void connectItemSingal(const VideoGridItemWidget* itemWidget) const;

signals:
    void itemDetailBtnClick();
    void itemDetailCheckBtnClick(bool isChecked);
    // void itemDownloadBtnClick(std::shared_ptr<BiliApi::VideoView> videoView);
public slots:
    signals:
    void handleDetialCheckBtnClick(bool isChecked);
};
