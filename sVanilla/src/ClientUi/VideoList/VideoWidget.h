#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "BiliApi/BiliApi.h"
#include <QStackedWidget>
#include <QWidget>

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
    bool processDetailsBtnClickEvent(QObject* watched);
    std::string detailSourceIdentifier;
    [[nodiscard]] bool detailPanelVisible() const;
    void showDetailPanel() const;
    void hideDetailPanel() const;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif  // VIDEOWIDGET_H
