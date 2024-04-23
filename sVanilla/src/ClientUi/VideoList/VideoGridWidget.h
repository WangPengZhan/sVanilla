#pragma once

#include <QWidget>
#include <QListWidget>
#include <QSplitter>
#include <QLabel>

#include "Adapter/BaseVideoView.h"

class VideoDetailWidget;

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

    [[nodiscard]] QSize sizeHint() const override;

private:
    void setUi();
    void signalsAndSlots();

public:
    std::string Identifier;
    std::shared_ptr<Adapter::BaseVideoView> m_videoView;

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

    void addVideoItem(const std::string& identifier);
    void updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView);
    void clearVideo();
    void getSignalPointer(QSplitter* splitter);

    Q_SIGNAL void downloandBtnClick(const std::shared_ptr<Adapter::BaseVideoView>& videoView);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void connectItemSingal(const VideoGridItemWidget* itemWidget);
    void showDetailPanel();
    void hideDetailPanel() const;
    [[nodiscard]] bool detailPanelVisible() const;
    [[nodiscard]] VideoDetailWidget* detailWidget() const;

    void adjustItemSize();

private:
    std::map<std::string, QListWidgetItem*> m_items;
    QSplitter* m_splitter = nullptr;
    std::string currentIdentifier;

    const int itemBaseWidth = 240;
    const int itemBaseHeight = 200;
    const float aspectRatio = static_cast<float>(itemBaseWidth) / itemBaseHeight;
};
