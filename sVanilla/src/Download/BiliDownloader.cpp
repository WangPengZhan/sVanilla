#include "BiliDownloader.h"

namespace download
{
BiliDownloader::BiliDownloader()
    : m_finished(false)
{
}
BiliDownloader::BiliDownloader(std::list<std::string> videoUris, std::list<std::string> audioUri, std::string path, std::string filename)
    : m_finished(false)
    , m_path(std::move(path))
    , m_filename(std::move(filename))
    , m_videoDownloader(videoUris, m_path)
    , m_audioDownloader(audioUri, m_path)
{
    std::string baseName = std::filesystem::path(m_filename).stem().string();
    m_videoDownloader.setFilename(baseName + "_video.mp4");
    m_audioDownloader.setFilename(baseName + "_audio.mp3");
}
void BiliDownloader::start()
{
}

void BiliDownloader::stop()
{
}

void BiliDownloader::pause()
{
}

void BiliDownloader::resume()
{
}

void BiliDownloader::downloadStatus()
{
}

void BiliDownloader::finish()
{
    // merge video and audio

    m_finished = true;

    // send finish signal

    // write to sqlite
}

void BiliDownloader::setVideoUris(const std::list<std::string>& videoUris)
{
    m_videoDownloader.setUris(videoUris);
}

void BiliDownloader::setAudioUris(const std::list<std::string>& audioUri)
{
    m_audioDownloader.setUris(audioUri);
}

void BiliDownloader::setPath(std::string path)
{
    m_path = std::move(path);
    m_audioDownloader.setPath(m_path);
    m_videoDownloader.setPath(m_path);
}

const std::string& BiliDownloader::path() const
{
    return m_path;
}

void BiliDownloader::setFilename(std::string filename)
{
    m_filename = std::move(filename);
    std::string baseName = std::filesystem::path(m_filename).stem().string();
    m_videoDownloader.setFilename(baseName + "_video.mp4");
    m_audioDownloader.setFilename(baseName + "_audio.mp3");
}

const std::string& BiliDownloader::filename() const
{
    return m_filename;
}

bool BiliDownloader::isFinished() const
{
    return m_finished;
}

std::string BiliDownloader::nowDownload() const
{
    return std::string();
}

}  // namespace download
