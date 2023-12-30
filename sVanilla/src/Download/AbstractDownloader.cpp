#include "AbstractDownloader.h"

namespace download
{

AbstractDownloader::Status AbstractDownloader::status() const
{
    return m_status;
}

}  // namespace download
