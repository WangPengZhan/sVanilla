#pragma once
#include <thread>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <string>
#include <memory>
#include <condition_variable>

namespace download
{

class AbstractDownloader;
class DownloadStatusThread
{
public:
    DownloadStatusThread();
    ~DownloadStatusThread();

    bool addTaks(std::shared_ptr<AbstractDownloader> downloader);

    void stop();

private:
    void downloadThread();

private:
    std::atomic_bool m_running;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::unordered_map<std::string, std::shared_ptr<AbstractDownloader>> m_downloadTasks;
    std::thread m_thread;
};

}  // namespace download
