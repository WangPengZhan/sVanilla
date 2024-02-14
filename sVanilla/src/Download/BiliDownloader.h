#pragma once
#include <string>
#include <list>

#include "AbstractDownloader.h"
#include "AriaDownloader.h"


namespace download
{

class BiliDownloader : public AbstractDownloader
{
public:
    BiliDownloader();
    BiliDownloader(std::list<std::string> videoUris, std::list<std::string> audioUri, 
        std::string path = "", std::string filename = "");
    ~BiliDownloader() = default;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    void downloadStatus() override;
    void finish() override;

    void setVideoUris(const std::list<std::string>& videoUris);
    void setAudioUris(const std::list<std::string>& audioUri);
    void setPath(std::string path);
    const std::string& path() const;
    void setFilename(std::string filename);
    const std::string& filename() const;

    bool isFinished() const;

    std::string nowDownload() const;

private:
    std::string m_path;
    std::string m_filename;
    std::string m_uniqueId;
    AriaDownloader m_videoDownloader;
    AriaDownloader m_audioDownloader;
    bool m_finished;
};

}  // namespace download
