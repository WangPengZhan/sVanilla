#pragma once

#include <QDialog>

namespace Ui { class VideoDetailWidget; };


class VideoDetailWidget : public QDialog
{
    Q_OBJECT

public:
    VideoDetailWidget(QWidget *parent = nullptr);
    ~VideoDetailWidget();

private:
    void SetUi();
    void SignalsAndSlots();

private:
    Ui::VideoDetailWidget* ui;
};
