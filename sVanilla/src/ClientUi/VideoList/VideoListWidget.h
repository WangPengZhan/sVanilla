#pragma once

#include "Adapter/BaseVideoView.h"
#include <QWidget>
#include <QListWidget>
#include <QSplitter>

class VideoDetailWidget;

namespace Ui
{
class VideoListItemWidget;
}

class VideoListItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoListItemWidget(std::string identifier, QWidget* parent = nullptr);
    ~VideoListItemWidget();

    void updateVideoItem();
    std::shared_ptr<Adapter::BaseVideoView> m_videoView;
    std::string Identifier;

private:
    void setUi();
    void signalsAndSlots();

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
    void addVideoItem(const std::string& identifier);
    void updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView);
    void clearVideo();
    void getSignalPointer(QSplitter* splitter);

private:
    void connectItemSingal(const VideoListItemWidget* itemWidget);
    void showDetailPanel();
    void hideDetailPanel();
    [[nodiscard]] bool detailPanelVisible() const;
    [[nodiscard]] VideoDetailWidget* detailWidget() const;

    std::map<std::string, QListWidgetItem*> m_items;
    QSplitter* m_splitter = nullptr;
    std::string currentIdentifier;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    // void resizeEvent(QResizeEvent* event) override;
};
