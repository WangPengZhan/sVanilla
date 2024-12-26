#include <QString>
#include <QStringList>
#include <QRegularExpression>

#include "UrlProcess.h"

namespace util
{

QString removeSpecialChar(const QString& path)
{
    QString temp = path;
    for (const auto& splicalChar : QString(specialChars))
    {
        temp.remove(splicalChar);
    }

    return temp;
}

std::string removeSpecialChar(const std::string& path)
{
    QString qPath = QString::fromStdString(path);
    return removeSpecialChar(qPath).toStdString();
}

}  // namespace util
