#pragma once

#include <QWidget>

namespace Adapter
{
struct BaseVideoView;
}
namespace Ui
{
class VideoDetailWidget;
}
struct VideoInfoFull;

class VideoDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoDetailWidget(QWidget* parent = nullptr);
    ~VideoDetailWidget();
    void updateUi(const std::shared_ptr<VideoInfoFull>& videoViewdata);

private:
    void setUi();
    void signalsAndSlots();

public:
    std::string m_bvid;

private:
    Ui::VideoDetailWidget* ui;
};
