#include <utility>
#include <vector>

#include "DownloadStatusThread.h"
#include "DownloadLog.h"
#include <Download/AbstractDownloader.h>

namespace download
{

DownloadStatusThread::DownloadStatusThread()
    : m_running(true)
    , m_thread(&DownloadStatusThread::downloadThread, this)
{
    DOWNLOAD_LOG_INFO("DownloadStatusThread thread created");
}

DownloadStatusThread::~DownloadStatusThread()
{
    stop();
    m_thread.join();
}

bool DownloadStatusThread::empty() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_downloadTasks.empty();
}

bool DownloadStatusThread::addTask(std::shared_ptr<AbstractDownloader> downloader)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto [iter, inserted] = m_downloadTasks.insert({downloader->guid(), downloader});
    if (!m_downloadTasks.empty())
    {
        m_condition.notify_one();
    }
    return inserted;
}

void DownloadStatusThread::stop()
{
    m_running.store(false);
    m_condition.notify_all();
}

void DownloadStatusThread::downloadThread()
{
    DOWNLOAD_LOG_INFO("DownloadStatusThread thread start");
    while (m_running.load())
    {
        std::vector<std::pair<std::string, std::shared_ptr<AbstractDownloader>>> tasks;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait(lock, [&] {
                return !m_downloadTasks.empty() || !m_running.load();
            });

            tasks.reserve(m_downloadTasks.size());
            for (const auto& task : m_downloadTasks)
            {
                tasks.push_back(task);
            }
        }

        std::vector<std::pair<std::string, std::shared_ptr<AbstractDownloader>>> removeTasks;
        for (const auto& [key, value] : tasks)
        {
            switch (value->status())
            {
            case AbstractDownloader::Ready:
            {
                value->start();
                break;
            }
            case AbstractDownloader::Downloading:
            {
                value->downloadStatus();
                break;
            }
            case AbstractDownloader::Pause:
            {
                value->pause();
                break;
            }
            case AbstractDownloader::Resumed:
            {
                value->resume();
                break;
            }
            case AbstractDownloader::Stopped:
            {
                value->stop();
                removeTasks.emplace_back(key, value);
                break;
            }
            case AbstractDownloader::Finished:
            {
                value->finish();
                removeTasks.emplace_back(key, value);
                break;
            }
            case AbstractDownloader::Error:
            default:
                break;
            }
        }

        if (!removeTasks.empty())
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (const auto& [key, value] : removeTasks)
            {
                auto iter = m_downloadTasks.find(key);
                if (iter != m_downloadTasks.end() && iter->second == value)
                {
                    m_downloadTasks.erase(iter);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }
    DOWNLOAD_LOG_INFO("DownloadStatusThread thread exit");
}

}  // namespace download
