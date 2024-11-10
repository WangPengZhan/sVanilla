#include "CoverUtil.h"

#include "NetWork/CNetWork.h"
#include "ClientLog.h"
#include "const_string.h"

#include <QString>
#include <QDir>

bool downloadCoverImage(const CoverInfo& coverInfo)
{
    const QString tempPath = QString::fromStdString(coverInfo.path);
    const QString fileName = QString::fromStdString(coverInfo.fileName);
    const QString fullPath = QDir::cleanPath(tempPath + QDir::separator() + fileName + ".jpg");

    QFile::remove(fullPath);

    std::string path = fullPath.toLocal8Bit().toStdString();
    FILE* file = fopen(path.c_str(), "wb");
    if (!file)
    {
        std::string str = strerror(errno);
        MLogE(svanilla::cDownloadModule, "fopen error: {}", str);
        return false;
    }
    network::NetWork netWork;
    netWork.get(coverInfo.url, file);
    fclose(file);

    return QFile::exists(fullPath);
}
