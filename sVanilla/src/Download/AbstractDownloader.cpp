#include "AbstractDownloader.h"

namespace download
{
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
