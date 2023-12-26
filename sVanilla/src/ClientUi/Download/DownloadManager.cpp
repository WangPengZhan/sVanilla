#include "DownloadManager.h"

AbstractDownloader::Status AbstractDownloader::status() const
{
    return m_status;
}

