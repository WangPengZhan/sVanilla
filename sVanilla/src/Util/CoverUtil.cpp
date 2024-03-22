#include "CoverUtil.h"
#include "NetWork/CNetWork.h"
#include <QString>
#include <QDir>

bool downloadCoverImage(const CoverInfo& coverInfo)
{
    const QString tempPath = QString::fromStdString(coverInfo.path);
    const QString fileName = QString::fromStdString(coverInfo.fileName);
    const QString fullPath = QDir::cleanPath(tempPath + QDir::separator() + fileName + ".jpg");
    if (!QFile::exists(fullPath))
    {
        FILE* file = fopen(fullPath.toStdString().c_str(), "wb");
        CNetWork netWork;
        netWork.HttpGet(coverInfo.url, file);
        fclose(file);
    }
    if (QFile::exists(fullPath))
    {
        return true;
    }
    return false;
}