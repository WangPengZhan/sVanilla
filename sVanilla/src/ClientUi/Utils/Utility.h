#pragma once
#include <QString>

namespace util
{

void showInFileExplorer(const QString& filePath);
void createMenu(QMenu* menu, int width, const std::list<std::string>& history, const std::function<void(const QString&)>&);

struct MoveStartEndValue
{
    QPoint startValue;
    QPoint endValue;
};

void moveAnimate(QObject* obj, MoveStartEndValue posChange, const std::function<void()>& = {});
}  // namespace util
