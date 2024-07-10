#pragma once

#include <QWidget>

namespace Adapter
{
struct BaseVideoView;
}
namespace Ui
{
class VideoInfoWidget;
}
struct VideoInfoFull;

class VideoInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoInfoWidget(QWidget* parent = nullptr);
    ~VideoInfoWidget();
    void setVidoInfo(std::shared_ptr<VideoInfoFull> infoFull);
    void updateUi();

private:
    void signalsAndSlots();

private:
    Ui::VideoInfoWidget* ui;
    std::shared_ptr<VideoInfoFull> m_infoFull;
};
