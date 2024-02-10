#pragma once

#include <QWidget>
#include <QListWidget>
namespace Ui
{
class VideoGridItemWidget;
}

class VideoGridItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoGridItemWidget(std::string bvid, QWidget* parent = nullptr);
    ~VideoGridItemWidget();
    void setCover();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::VideoGridItemWidget* ui;
    std::string m_bvid;

signals:
    void detailBtnClick();
};

class VideoGridWidget : public QListWidget
{
    Q_OBJECT

public:
    VideoGridWidget(QWidget* parent = nullptr);
    ~VideoGridWidget();

    void addVideoItem(const std::string& bvid);
    void setCover();

private:
    std::map<std::string, QListWidgetItem*> m_items;

signals:
    void itemDetailBtnClick();
};
