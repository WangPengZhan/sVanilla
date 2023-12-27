#pragma once

#include <QDialog>

namespace Ui
{
class VideoDetailWidget;
}

class VideoDetailWidget : public QDialog
{
    Q_OBJECT

public:
    explicit VideoDetailWidget(QWidget* parent = nullptr);
    ~VideoDetailWidget() override;

private:
    void SetUi();
    void SignalsAndSlots();

private:
    Ui::VideoDetailWidget* ui;
};
