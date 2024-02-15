#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "Adapter/BaseVideoView.h"
#include "VideoDetailWidget.h"
#include "BiliApi/BiliApi.h"
#include "Theme/GeometryAnimation.h"
#include <QStackedWidget>
#include <QWidget>
#include <QtWidgets/QPushButton>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
namespace Ui
{
class VideoPage;
}
QT_END_NAMESPACE

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget() override;

    void addVideoItem(const std::string& bvid) const;
    void updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView);
    void updateDetailPanel(const std::shared_ptr<Adapter::BaseVideoView>& videoView) const;
    void clearVideo();

private:
    Ui::VideoPage* ui;
    std::shared_ptr<GeometryAnimation> detailAnimation;
    bool processDetailsBtnClickEvent(QObject* watched);
    bool precessDownloadBtnClickEvent(QObject* watched);
    void installBtnEventFilter();
    std::list<QPushButton*> eventBtns;
    std::string detailSourceIdentifier;

    [[nodiscard]] VideoDetailWidget* detailPanel() const;
    [[nodiscard]] bool detailPanelVisible() const;
    void showDetailPanel() const;
    void hideDetailPanel() const;
    void updateDetailPanelWidth();

signals:
    void downloadBtnClick(const std::shared_ptr<Adapter::BaseVideoView>& videoView);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif  // VIDEOWIDGET_H
