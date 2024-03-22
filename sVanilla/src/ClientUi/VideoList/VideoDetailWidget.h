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

class VideoDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoDetailWidget(QWidget* parent = nullptr);
    ~VideoDetailWidget();
    void updateUi(const std::shared_ptr<Adapter::BaseVideoView>& videoViewdata);

private:
    void setUi();
    void signalsAndSlots();

public:
    std::string m_bvid;

private:
    Ui::VideoDetailWidget* ui;
};
