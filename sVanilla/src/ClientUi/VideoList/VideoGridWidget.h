#pragma once

#include <QWidget>
#include <QListWidget>
#include <QSplitter>
#include <QLabel>

#include "Adapter/BaseVideoView.h"

class Spiner;
constexpr int itemBaseWidth = 240;
constexpr int itemBaseHeight = 200;
constexpr float aspectRatio = static_cast<float>(itemBaseWidth) / static_cast<float>(itemBaseHeight);

class VideoDetailWidget;
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
    explicit VideoGridItemWidget(std::string identifier, QWidget* parent = nullptr);
    ~VideoGridItemWidget();
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

public:
    std::string m_identifier;
    std::shared_ptr<VideoInfoFull> m_videoView;

private:
    Ui::VideoGridItemWidget* ui;

signals:
    void detailBtnClick();
    void downloadBtnClick();
    void detailCheckBtnClick(bool isChecked);
};

class VideoGridWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit VideoGridWidget(QWidget* parent = nullptr);

    void addVideoItem(const std::shared_ptr<VideoInfoFull>& videoView);
    void clearVideo();
    void showDetailPanel();
    void hideDetailPanel() const;
    void getSignalPointer(QSplitter* splitter);
    void coverReady(const std::string& id) const;

    Q_SIGNAL void downloandBtnClick(const std::shared_ptr<VideoInfoFull>& videoView);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void connectItemSingal(const VideoGridItemWidget* itemWidget);
    [[nodiscard]] bool detailPanelVisible() const;
    [[nodiscard]] VideoDetailWidget* detailWidget() const;

    void adjustItemSize();
    void setItemSize(const QSize& size);

private:
    std::unordered_map<std::string, QListWidgetItem*> m_items;
    QSplitter* m_splitter = nullptr;
    std::string currentIdentifier;
};
