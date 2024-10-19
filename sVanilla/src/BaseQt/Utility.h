#pragma once
#include <QString>

namespace util
{

void showInFileExplorer(const QString& filePath);
void createMenu(QMenu* menu, int width, const std::vector<std::string>& history, const std::function<void(const QString&)>&);

struct AnimationStartEnd
{
    QVariant start;
    QVariant end;
};

void animate(QObject* obj, AnimationStartEnd change, const QByteArray& propertyName = "pos", const std::function<void()>& = {}, int duration = 300);
}  // namespace util
