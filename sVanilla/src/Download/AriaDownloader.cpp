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
AriaDownloader::AriaDownloader(std::list<std::string> uris, const aria2net::AriaSendOption& option)
    : m_ariaSendOption(option)
    , m_uris(std::move(uris))
    , m_finished(false)
{
}

void AriaDownloader::start()
{
    aria2net::AriaAddUri result = aria2net::AriaClient::globalClient().AddUriAsync(m_uris, m_ariaSendOption);
    if (result.error.code != 0)
    {
        // to do
        m_status = Error;
        return;
    }

    m_gid = result.result;
    m_status = Downloading;
}

void AriaDownloader::stop()
{
    aria2net::AriaClient::globalClient().RemoveAsync(m_gid);
    m_status = Waitting;
}

void AriaDownloader::pause()
{
    aria2net::AriaClient::globalClient().PauseAsync(m_gid);
    m_status = Paused;
}

void AriaDownloader::resume()
{
    aria2net::AriaClient::globalClient().UnpauseAsync(m_gid);
    m_status = Downloading;
}

void AriaDownloader::downloadStatus()
{
    m_downloadTellStatus = aria2net::AriaClient::globalClient().TellStatus(m_gid);
    qDebug() << "Aria2Client tellStatus response: " << m_downloadTellStatus.toString().c_str();
    if (m_downloadTellStatus.error.code != 0
        || (!m_downloadTellStatus.result.errorCode.empty() && m_downloadTellStatus.result.errorCode != "0"))
    {
        m_status = Error;
        return;
    }
    try
    {
        m_info.complete = std::stoull(m_downloadTellStatus.result.completedLength);
        m_info.speed = std::stoull(m_downloadTellStatus.result.downloadSpeed);
        m_info.total = std::stoull(m_downloadTellStatus.result.totalLength);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    if (m_info.complete == m_info.total && m_downloadTellStatus.result.status == "complete")
    {
        m_status = Finished;
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
