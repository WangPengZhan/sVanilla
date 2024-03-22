#pragma once

#include "Adapter/BaseVideoView.h"
#include <QWidget>
#include <QListWidget>
#include <QSplitter>
#include <QtWidgets/QLabel>

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
    explicit VideoGridItemWidget(std::string bvid, QWidget* parent = nullptr);
    ~VideoGridItemWidget();
    void setCover(const std::string& id);
    void updateVideoCard() const;
    std::shared_ptr<Adapter::BaseVideoView> m_videoView;

    [[nodiscard]] QSize sizeHint() const override;
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

    void addVideoItem(const std::string& identifier);
    void updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView);
    void clearVideo();
    void getSignalPointer(QSplitter* splitter);

private:
    void connectItemSingal(const VideoGridItemWidget* itemWidget);
    void showDetailPanel();
    void hideDetailPanel() const;
    [[nodiscard]] bool detailPanelVisible() const;
    [[nodiscard]] VideoDetailWidget* detailWidget() const;

    std::map<std::string, QListWidgetItem*> m_items;
    QSplitter* m_splitter = nullptr;
    std::string currentIdentifier;

};
