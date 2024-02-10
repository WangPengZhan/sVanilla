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

    void addVideoItem(const std::string& bvid);
    void updateVideoItem(const std::shared_ptr<BiliApi::VideoView>& videoView);
public slots:
    void showDetailPanel();

private:
    Ui::VideoPage* ui;
};

#endif  // VIDEOWIDGET_H
