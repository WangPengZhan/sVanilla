#pragma once
#include "Aria2Net/Protocol/Protocol.h"
#include <string>
#include <atomic>

namespace download
{

struct DownloadInfo
{
    std::size_t total = 0;
    std::size_t complete = 0;
    std::size_t speed = 0;
    std::string stage;
};

struct ResourseInfo
{
    std::list<std::string> videoUris;
    std::list<std::string> audioUris;
    aria2net::AriaSendOption option;
};

class AbstractDownloader
{
public:
    enum Status
    {
        Waitting,
        Ready,
        Downloading,
        Pause,
        Paused,
        Resumed,
        Stopped,
        Finished,
        Error
    };

    AbstractDownloader() = default;
    virtual ~AbstractDownloader() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void downloadStatus() = 0;
    virtual void finish() = 0;

    const DownloadInfo& info() const;
    const std::string& stage() const;

    std::string guid() const;
    void setStatus(Status status);
    Status status() const;

protected:
    std::atomic<Status> m_status = Waitting;
    std::string m_guid;
    std::string m_stage;
    DownloadInfo m_info;
};

}  // namespace download
