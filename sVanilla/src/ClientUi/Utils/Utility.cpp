#include <QProcess>
#include <QDir>

#include "ClientUi/Utils/Utility.h"

namespace util
{

void showInFileExplorer(const QString& filePath)
{
    QStringList arguments;

#ifdef _WIN32
    QString explorerCommand = "explorer";
    arguments << "/select," << QDir::toNativeSeparators(filePath);
#elif __linux__
    QString explorerCommand = "nautilus";
    arguments << filePath;
#elif __APPLE__
    QString explorerCommand = "open";
    arguments << QStringLiteral("-R") << filePath;
#endif

    QProcess::startDetached(explorerCommand, arguments);
}

}  // namespace util
