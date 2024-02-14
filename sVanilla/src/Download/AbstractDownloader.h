#pragma once
#include <string>
#include <atomic>
namespace download
{

class AbstractDownloader
{
public:
    enum Status
    {
        Ready,
        Downloading,
        Paused,
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

    std::string guid() const;
    void setStatus(Status status);
    Status status() const;

protected:
    std::atomic<Status> m_status;
    std::string m_guid;
};

}  // namespace download
