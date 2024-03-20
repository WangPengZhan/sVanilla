#include "BiliDownloader.h"
#include "Sqlite/SQLiteManager.h"
#include "FFmpeg//FFmpegHelper.h"

namespace download
{
BiliDownloader::BiliDownloader()
    : m_finished(false)
{
    m_videoDownloader.setStatus(Ready);
    m_audioDownloader.setStatus(Ready);
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
    m_videoDownloader.setStatus(Ready);
    m_audioDownloader.setStatus(Ready);
}

void BiliDownloader::start()
{
    m_videoDownloader.start();
    m_audioDownloader.start();
    m_info.stage = "audio/video downloading";
    m_status = Downloading;
}

void BiliDownloader::stop()
{
    m_videoDownloader.stop();
    m_audioDownloader.stop();
    m_status = Wait;
}

void BiliDownloader::pause()
{
    m_videoDownloader.pause();
    m_audioDownloader.pause();
    m_status = Wait;
}

void BiliDownloader::resume()
{
    m_videoDownloader.resume();
    m_audioDownloader.resume();
    m_status = Downloading;
}

void BiliDownloader::downloadStatus()
{
    if (m_audioDownloader.status() == Downloading)
    {
        m_audioDownloader.downloadStatus();
    }

    if (m_videoDownloader.status() == Downloading)
    {
        m_videoDownloader.downloadStatus();
    }

    auto video = m_videoDownloader.info();
    auto audio = m_audioDownloader.info();

    m_info.total = video.total + audio.total;
    m_info.complete = video.complete + audio.complete;
    m_info.speed = video.speed + audio.speed;

    if (m_info.total == m_info.complete)
    {
        ffmpeg::MergeInfo merge;
        merge.audio = m_audioDownloader.path() + "/" + m_audioDownloader.filename();
        merge.video = m_videoDownloader.path() + "/" + m_videoDownloader.filename();
        merge.targetVideo = path() + "/" + filename();
        m_info.stage = "ffmpeg mixed!";
        
        ffmpeg::FFmpegHelper::mergeVideo(merge);

        m_status = Finished;
    }
}

void BiliDownloader::finish()
{
    m_finished = true;

    // send finish signal

    // write to sqlite
    auto finishedItem = SQLite::SQLiteManager::getInstance().finishedItemTable();
    download::FinishedItem item;
    finishedItem.insertItem({item});
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
