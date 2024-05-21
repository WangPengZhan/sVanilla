#pragma once
#include <QWidget>
#include <QPainter>

class RoundImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoundImageWidget(QWidget* parent = nullptr);

    void setPixmap(const QPixmap& pixmap);
    void resizePixmap();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap m_pixmap;
    QPixmap m_originalPixmap;

    constexpr static int m_radius = 10;
};
