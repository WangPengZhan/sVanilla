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
    void updateUi(const std::shared_ptr<VideoInfoFull>& infoFull);

    Q_SIGNAL void fileNameEditingFinished(const QString& fileName);

private:
    void signalsAndSlots();

private:
    Ui::VideoInfoWidget* ui;
};
