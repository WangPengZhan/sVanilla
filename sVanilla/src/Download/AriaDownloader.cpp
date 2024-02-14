#include "AriaDownloader.h"
#include "Aria2Net/AriaClient/AriaClient.h"

namespace download
{
AriaDownloader::AriaDownloader()
    : m_finished(false)
{
}

AriaDownloader::AriaDownloader(std::list<std::string> uris, std::string path, std::string filename)
    : m_finished(false)
    , m_uris(std::move(uris))
{
    m_ariaSendOption.dir = std::move(path);
    m_ariaSendOption.out = std::move(filename);
}

void AriaDownloader::start()
{
    aria2net::AriaAddUri result = aria2net::AriaClient::globalClient().AddUriAsync(m_uris, m_ariaSendOption);
    if (result.error.code != 0)
    {
        // to do

        return;
    }

    m_gid = result.result;
}

void AriaDownloader::stop()
{
    aria2net::AriaClient::globalClient().RemoveAsync(m_gid);
}

void AriaDownloader::pause()
{
    aria2net::AriaClient::globalClient().PauseAsync(m_gid);
}

void AriaDownloader::resume()
{
    aria2net::AriaClient::globalClient().UnpauseAsync(m_gid);
}

void AriaDownloader::downloadStatus()
{
    m_downloadTellStatus = aria2net::AriaClient::globalClient().TellStatus(m_gid);
    if (m_downloadTellStatus.error.code != 0)
    {
        // to do
        return;
    }
}

void AriaDownloader::finish()
{
}

const aria2net::AriaSendOption& AriaDownloader::sendOption() const
{
    return m_ariaSendOption;
}

void AriaDownloader::setAriaSendOption(const aria2net::AriaSendOption& option)
{
    m_ariaSendOption = option;
}

const std::string& AriaDownloader::path() const
{
    return m_ariaSendOption.dir;
}

void AriaDownloader::setPath(const std::string& path)
{
    m_ariaSendOption.dir = path;
}

const std::string& AriaDownloader::filename() const
{
    return m_ariaSendOption.out;
}

void AriaDownloader::setFilename(const std::string& filename)
{
    m_ariaSendOption.out = filename;
}

void AriaDownloader::setUris(const std::list<std::string>& uris)
{
    m_uris = uris;
}

const std::list<std::string>& AriaDownloader::uris() const
{
    return m_uris;
}

const std::string& AriaDownloader::gid() const
{
    return m_gid;
}

const std::size_t& AriaDownloader::fileSize() const
{
    return m_fileSize;
}

const aria2net::AriaTellStatus& AriaDownloader::nowStatus()
{
    return m_downloadTellStatus;
}

}  // namespace download
