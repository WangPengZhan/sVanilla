#pragma once
#include <QObject>
#include <QPropertyAnimation>

class GeometryAnimation : public QObject
{
    Q_OBJECT
public:
    explicit GeometryAnimation(QObject* parent = nullptr);

    void setDuration(int duration);
    void setTargetObject(QObject* target) const;
    void setValues(const QVariant& start, const QVariant& end) const;
    void start() const;
private:
    QPropertyAnimation* geometryAnimation;
    int m_duration = 300;

signals:
    void finish();
};
