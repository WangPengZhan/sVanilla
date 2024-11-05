#pragma once

#include <QWidget>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DownloadTip;
}  //  namespace Ui
QT_END_NAMESPACE

class DownloadTip : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadTip(QWidget* parent = nullptr);

    void setDragRect(QRect rect = {});
    QRect dragRect() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::DownloadTip* ui;
    bool dragging = false;
    QPoint lastMousePosition;
    QRect m_rect;
};