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
    explicit VideoDetailWidget(std::string bvid,QWidget* parent = nullptr);
    ~VideoDetailWidget();

private:
    void setUi();
    void signalsAndSlots();

    std::string m_bvid;

private:
    Ui::VideoDetailWidget* ui;
};
