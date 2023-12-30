#pragma once
#include <thread>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <string>

class AbstractDownloader;

namespace download
{

class AbstractDownloader;
class DownloadStatusThread
{
public:
    DownloadStatusThread();
    ~DownloadStatusThread();

    void addTaks();

    void stop();

private:
    void downloadThread();

private:
    std::atomic_bool m_running;
    std::mutex m_mutex;
    std::unordered_map<std::string, AbstractDownloader*> m_downloadTasks;
    std::thread m_thread;
};

}  // namespace download
