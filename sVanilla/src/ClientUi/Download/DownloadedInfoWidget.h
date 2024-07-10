
#ifndef DOWNLOADEDINFOWIDGET_H
#define DOWNLOADEDINFOWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DownloadedInfoWidget;
}
QT_END_NAMESPACE

struct VideoInfoFull;

class DownloadedInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadedInfoWidget(QWidget* parent = nullptr);
    ~DownloadedInfoWidget() override;

    void updateUi(const std::shared_ptr<VideoInfoFull>& videoInfo);

private:
    void signalsAndSlots();

private:
    Ui::DownloadedInfoWidget* ui;
};

#endif  // DOWNLOADEDINFOWIDGET_H
