#include "DownloadManager.h"

template <typename T> QString formatSize(T bytesPerSec)
{
    const double Gib = pow(2, 30);
    const double Mib = pow(2, 20);
    const double Kib = pow(2, 10);
    if (bytesPerSec >= Gib)
    {
        return QString::number(bytesPerSec / Gib, 'g', 2) + "GiB";
    }
    if (bytesPerSec >= Mib)
    {
        return QString::number(bytesPerSec / Mib, 'g', 2) + "Mib";
    }
    if (bytesPerSec >= Kib)
    {
        return QString::number(bytesPerSec / Kib, 'g', 2) + "Kib";
    }
    return QString::number(bytesPerSec, 'g', 3) + "B";
}

DownloadManager::DownloadManager(QObject* parent)
    : QObject(parent)
{
}
void DownloadManager::addDownloadGID(const std::string& gid)
{
    // emit Event::getInstance()->AddDownloadTask(gid);

    downloadGIDs.push_back(gid);
}
