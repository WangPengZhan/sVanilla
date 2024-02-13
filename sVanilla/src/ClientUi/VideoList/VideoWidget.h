#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "VideoDetailWidget.h"
#include "BiliApi/BiliApi.h"
#include "Theme/GeometryAnimation.h"

#include <QStackedWidget>
#include <QWidget>
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
    void updateVideoItem(const std::shared_ptr<BiliApi::VideoView>& videoView) const;
    void updateDetailPanel(const std::string& detail) const;

private:
    Ui::VideoPage* ui;
    std::shared_ptr<GeometryAnimation> detailAnimation;
    bool processDetailsBtnClickEvent(QObject* watched);
    std::string detailSourceIdentifier;

    [[nodiscard]] VideoDetailWidget* detailPanel() const;
    [[nodiscard]] bool detailPanelVisible() const;
    void showDetailPanel() const;
    void hideDetailPanel() const;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif  // VIDEOWIDGET_H
