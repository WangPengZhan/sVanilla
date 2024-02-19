#pragma once

#include "Adapter/BaseVideoView.h"
#include <QStackedWidget>

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
    void clearVideo() const;

private:
    Ui::VideoPage* ui;

};

