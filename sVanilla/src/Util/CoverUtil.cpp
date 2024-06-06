#include "CoverUtil.h"
#include "NetWork/CNetWork.h"
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
    network::NetWork netWork;
    netWork.get(coverInfo.url, file);
    fclose(file);

    return QFile::exists(fullPath);
}