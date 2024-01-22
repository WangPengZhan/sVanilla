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
    ~VideoDetailWidget();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::VideoDetailWidget* ui;
};
