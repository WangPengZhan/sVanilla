#include "GeometryAnimation.h"

GeometryAnimation::GeometryAnimation(QObject* parent)
    : QObject(parent)
    , geometryAnimation(new QPropertyAnimation(nullptr, "geometry"))
{
    geometryAnimation->setDuration(m_duration);
    geometryAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(geometryAnimation, &QPropertyAnimation::finished, this, &GeometryAnimation::finish);
}
void GeometryAnimation::setDuration(const int duration)
{
    m_duration = duration;
}
void GeometryAnimation::setTargetObject(QObject* target) const
{
    if (!target)
    {
        return;
    }
    geometryAnimation->setTargetObject(target);
}
void GeometryAnimation::setValues(const QVariant& start, const QVariant& end) const
{
    geometryAnimation->setStartValue(start);
    geometryAnimation->setEndValue(end);
}
void GeometryAnimation::start() const
{
    geometryAnimation->start();
}