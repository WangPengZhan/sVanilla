#pragma once
#include <QMouseEvent>
#include <QPainterPath>
#include <QWidget>
#include <QPropertyAnimation>
#include <QPaintEvent>

class HorizonNavigation final : public QWidget {
    Q_OBJECT
public:
    explicit HorizonNavigation(QWidget *parent = nullptr, QStringList items = {});
    void addItems(const QStringList &items);

protected:
    void paintEvent(QPaintEvent *event) override;
    //    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private:
    QStringList itemList;
    int rowHeight = 26;
    int columnWidth = 100;
    int radius = 4;
    int offset = 0;
    int currentIndex = 0;
    int preIndex = 0;
    QPropertyAnimation *animation;
    void onValeChanged(const QVariant &v);
    void setUi();
signals:
    void currentItemChanged(const int &index);
};