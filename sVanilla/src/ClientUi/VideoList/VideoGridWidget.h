#pragma once

#include <QWidget>
#include <QListWidget>
#include <QSplitter>
#include <QLabel>

#include "Adapter/BaseVideoView.h"

class VideoGridWidget;
class Spiner;
constexpr int itemBaseWidth = 240;
constexpr int itemBaseHeight = 200;
constexpr float aspectRatio = static_cast<float>(itemBaseWidth) / static_cast<float>(itemBaseHeight);

class VideoInfoWidget;
struct VideoInfoFull;

namespace Ui
{
class VideoGridItemWidget;
}

void elideText(QLabel* label, const QString& text);

class VideoGridItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoGridItemWidget(QWidget* parent = nullptr);
    ~VideoGridItemWidget();

    void setGridWidget(VideoGridWidget* gridWidget, QListWidgetItem* widgetItem);
    void setVideoInfo(const std::shared_ptr<VideoInfoFull>& infoFull);

    void setCover();
    void updateVideoCard();
    void updateCover();

    [[nodiscard]] QSize sizeHint() const override;

protected:
    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

    void showInfoPanel() const;
    void downloadItem() const;

private:
    std::shared_ptr<VideoInfoFull> m_infoFull;
    VideoGridWidget* m_gridWidget = nullptr;
    QListWidgetItem* m_listWidgetItem = nullptr;
    Ui::VideoGridItemWidget* ui;

};

class VideoGridWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit VideoGridWidget(QWidget* parent = nullptr);

    void addVideoItem(const std::shared_ptr<VideoInfoFull>& videoView);
    void clearVideo();

    void setInfoPanelSignalPointer(VideoInfoWidget* infoWidget, QSplitter* splitter);
    void showInfoPanel(int index);
    void updateInfoPanel(const std::shared_ptr<VideoInfoFull>& infoFull) const;
    void coverReady(int id) const;

    Q_SIGNAL void downloandBtnClick(const std::shared_ptr<VideoInfoFull>& infoFull);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void adjustItemSize() const;
    void setItemSize(const QSize& size) const;

private:
    QSplitter* m_splitter = nullptr;
    VideoInfoWidget* m_infoWidget = nullptr;
    int previousRow = -1;
};
