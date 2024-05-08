#pragma once

#include <QWidget>
#include <QListWidget>
#include <QSplitter>

#include "Adapter/BaseVideoView.h"

class VideoListWidget;
class VideoInfoWidget;
namespace Ui
{
class VideoListItemWidget;
}
struct VideoInfoFull;

class VideoListItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoListItemWidget(QWidget* parent = nullptr);
    ~VideoListItemWidget();

    void setListWidget(VideoListWidget* listWidget);
    void setVideoInfo(const std::shared_ptr<VideoInfoFull>& infoFull);

    void updateVideoItem();

private:
    void signalsAndSlots();

    void showInfoPanel() const;
    void downloadItem() const;

private:
    std::shared_ptr<VideoInfoFull> m_infoFull;
    VideoListWidget* m_listWidget = nullptr;
    Ui::VideoListItemWidget* ui;
};

class VideoListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit VideoListWidget(QWidget* parent = nullptr);
    void addVideoItem(const std::shared_ptr<VideoInfoFull>& infoFull);
    void clearVideo();

    void setInfoPanelSignalPointer(VideoInfoWidget* infoWidget, QSplitter* splitter);
    void showInfoPanel();
    void updateInfoPanel(const std::shared_ptr<VideoInfoFull>& infoFull) const;

    Q_SIGNAL void downloandBtnClick(const std::shared_ptr<VideoInfoFull>& infoFull);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QSplitter* m_splitter = nullptr;
    VideoInfoWidget* m_infoWidget = nullptr;
    int previousRow = -1;
};
