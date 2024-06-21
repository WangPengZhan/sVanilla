#pragma once

#include <QWidget>

class DividingLine : public QWidget
{
    Q_OBJECT
public:
    explicit DividingLine(QWidget* parent);

    void setLineWidth(qreal width);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    qreal lineWidth = 1;
};
