#pragma once
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QBrush>
#include <QLabel>
#include <QImage>
#include <QPainterPath>

class RoundedLabel : public QLabel {
    Q_OBJECT
public:
    explicit RoundedLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
        : QLabel(parent, f) {}

    void setPixmap(const QPixmap &pixmap, int cornerRadius) {
        QPixmap roundedPixmap(pixmap.size());
        roundedPixmap.fill(Qt::transparent);

        QPainter painter(&roundedPixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QPainterPath path;
        path.addRoundedRect(pixmap.rect(), cornerRadius, cornerRadius);
        painter.setClipPath(path);
        painter.drawPixmap(pixmap.rect(), pixmap);
        painter.end();

        this->originalPixmap = roundedPixmap;
        QLabel::setPixmap(this->originalPixmap.scaled(QSize(300,200), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        if (!this->originalPixmap.isNull()) {
            QLabel::setPixmap(this->originalPixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        QLabel::resizeEvent(event);
    }

private:
    QPixmap originalPixmap;
};
