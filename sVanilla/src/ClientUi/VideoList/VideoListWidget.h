#pragma once

#include <QWidget>
#include <QListWidget>
#include <QSplitter>

#include "Adapter/BaseVideoView.h"

class VideoDetailWidget;
namespace Ui
{
class VideoListItemWidget;
}
struct VideoInfoFull;

class VideoListItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoListItemWidget(std::string identifier, QWidget* parent = nullptr);
    ~VideoListItemWidget();

    void updateVideoItem();

private:
    void setUi();
    void signalsAndSlots();

public:
    std::string m_identifier;
    std::shared_ptr<VideoInfoFull> m_videoView;

private:
    Ui::VideoListItemWidget* ui;

signals:
    void detailBtnClick();
    void detailCheckBtnClick(bool isChecked);
};

class VideoListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit VideoListWidget(QWidget* parent = nullptr);
    void addVideoItem(const std::shared_ptr<VideoInfoFull>& videoView);
    void clearVideo();
    void getSignalPointer(QSplitter* splitter);

private:
    void connectItemSingal(const VideoListItemWidget* itemWidget);
    void showDetailPanel();
    void hideDetailPanel();
    [[nodiscard]] bool detailPanelVisible() const;
    [[nodiscard]] VideoDetailWidget* detailWidget() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    std::map<std::string, QListWidgetItem*> m_items;
    QSplitter* m_splitter = nullptr;
    std::string currentIdentifier;
};
