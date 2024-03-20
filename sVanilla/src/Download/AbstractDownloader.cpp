#include "AbstractDownloader.h"

namespace download
{
const DownloadInfo& AbstractDownloader::info() const
{
    return m_info;
}

const std::string& AbstractDownloader::stage() const
{
    return m_stage;
}

std::string AbstractDownloader::guid() const
{
    return m_guid;
}

void AbstractDownloader::setStatus(Status status)
{
    m_status.store(status);
}

AbstractDownloader::Status AbstractDownloader::status() const
{
    return m_status.load();
}

}  // namespace download
