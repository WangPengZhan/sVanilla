#pragma once

#include "AbstractDownloader.h"
#include "Aria2Net/Protocol/Protocol.h"

namespace download
{

class AriaDownloader : public AbstractDownloader
{
public:
    AriaDownloader();
    AriaDownloader(std::list<std::string> uris, std::string path = "", std::string filename = "");
    explicit AriaDownloader(std::list<std::string> uris, const aria2net::AriaSendOption& option);
    ~AriaDownloader() = default;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    void downloadStatus() override;
    void finish() override;

    const aria2net::AriaSendOption& sendOption() const;
    void setAriaSendOption(const aria2net::AriaSendOption& option);
    const std::string& path() const;
    void setPath(const std::string& path);
    const std::string& filename() const;
    void setFilename(const std::string& filename);
    void setUris(const std::list<std::string>& uris);
    const std::list<std::string>& uris() const;
    const std::string& gid() const;
    const std::size_t& fileSize() const;
    const aria2net::AriaTellStatus& nowStatus();

private:
    std::string m_gid;
    aria2net::AriaSendOption m_ariaSendOption;
    std::list<std::string> m_uris;
    std::size_t m_fileSize{};
    aria2net::AriaTellStatus m_downloadTellStatus;
    bool m_finished;
};

}  // namespace download
