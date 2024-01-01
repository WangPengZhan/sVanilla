#pragma once

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

    Status status() const;

protected:
    Status m_status;
};

}  // namespace download
