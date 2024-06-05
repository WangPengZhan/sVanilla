#pragma once
#include <QString>

namespace util
{

void showInFileExplorer(const QString& filePath);
void createMenu(QMenu* menu, int width, const std::vector<std::string>& history, const std::function<void(const QString&)>&);

struct MoveStartEndValue
{
    QPoint startValue;
    QPoint endValue;
};

void moveAnimate(QObject* obj, MoveStartEndValue posChange, const std::function<void()>& = {});

struct LayoutMoveStartEndValue
{
    int startValue;
    int endValue;
};

void moveAnimate(QObject* obj, LayoutMoveStartEndValue posChange, const std::function<void()>& = {});

struct AnimationStartEnd
{
    QVariant start;
    QVariant end;
};

void animate(QObject* obj, AnimationStartEnd change, const QByteArray& propertyName = "pos", const std::function<void()>& = {});
}  // namespace util
