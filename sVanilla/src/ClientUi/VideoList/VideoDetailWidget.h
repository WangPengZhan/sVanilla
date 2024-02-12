#pragma once

#include <QWidget>

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
    void updateUi(const std::string& data);

private:
    void setUi();
    void signalsAndSlots();

    std::string m_bvid;

private:
    Ui::VideoDetailWidget* ui;
};
