#pragma once

#include <QWidget>
#include <QListWidget>
#include <QSplitter>
#include <QLabel>

#include "Adapter/BaseVideoView.h"

struct InfoPanelData;
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
    struct CardInfo
    {
        QString title;
        QString author;
        QString duration;
        QString publishDate;
    };

    explicit VideoGridItemWidget(QWidget* parent = nullptr);
    ~VideoGridItemWidget();

    void saveWidgetItem(QListWidgetItem* widgetItem);
    void setVideoInfo(const std::shared_ptr<VideoInfoFull>& infoFull);
    std::shared_ptr<VideoInfoFull> getVideoInfo();
    void setCover();
    void updateVideoCard();
    void updateCover();

    const CardInfo& getCardInfo() const;
    QListWidgetItem* getWidgetItem() const
    {
        return m_listWidgetItem;
    }

    [[nodiscard]] QSize sizeHint() const override;

signals:
    void downloadTrigger();
    void showInfoTigger();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

    void updateCard();
    void createContextMenu();

private:
    Ui::VideoGridItemWidget* ui;
    std::shared_ptr<VideoInfoFull> m_infoFull;
    QListWidgetItem* m_listWidgetItem = nullptr;
    QMenu* m_menu = nullptr;
    CardInfo m_cardInfo;
};

class VideoGridWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit VideoGridWidget(QWidget* parent = nullptr);

    void addVideoItem(const std::shared_ptr<VideoInfoFull>& videoView);
    void clearVideo();

    void coverReady(int id) const;
    void updateFileName(const QString& fileName);
    void updateCovers();

    VideoGridItemWidget* getItem(QListWidgetItem* item) const;
    [[nodiscard]] VideoGridItemWidget* getItem(int index) const;
    std::vector<VideoGridItemWidget*> getItems() const;
    std::vector<QListWidgetItem*> getWidgetItems() const;
    std::vector<std::shared_ptr<VideoInfoFull>> getVideoInfo() const;

signals:
    void downloandBtnClick(const std::shared_ptr<VideoInfoFull>&);
    void infoBtnClick(const InfoPanelData&);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setUi();
    void setItemShortCuts();

    void adjustItemSize() const;
    void setItemSize(const QSize& size) const;

    void downloadAllItem();
    void downloadSelectedItem();
    void downloadItem(QListWidgetItem* item);
    void showInfo(QListWidgetItem* widget);

private:
    int previousRow = -1;
};
