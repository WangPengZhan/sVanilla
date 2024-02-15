#pragma once
#include <QWidget>
#include <QPainter>

class RoundImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoundImageWidget(QWidget* parent = nullptr, const QPixmap& pixmap = QPixmap());

    void setPixmap(const QPixmap& pixmap);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap m_pixmap;
};
