#include <vector>

#include "DownloadStatusThread.h"
#include "AbstractDownloader.h"

namespace download
{

DownloadStatusThread::DownloadStatusThread()
    : m_running(true)
    , m_thread(&DownloadStatusThread::downloadThread, this)
{
}

DownloadStatusThread::~DownloadStatusThread()
{
    stop();
    m_thread.join();
}

bool DownloadStatusThread::addTaks(std::shared_ptr<AbstractDownloader> downloader)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto [iter, inserted] = m_downloadTasks.insert({downloader->guid(), downloader});
    if (m_downloadTasks.size() == 1)
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
    while (m_running.load())
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [&] {
            return !m_downloadTasks.empty();
        });
        std::vector<std::string> removeKeys;
        for (auto& [key, value] : m_downloadTasks)
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
            case AbstractDownloader::Paused:
            {
                value->pause();
                removeKeys.push_back(key);
                break;
            }
            case AbstractDownloader::Stopped:
            {
                value->stop();
                removeKeys.push_back(key);
                break;
            }
            case AbstractDownloader::Finished:
            case AbstractDownloader::Error:
            {
                removeKeys.push_back(key);
                break;
            }
            default:
                break;
            }
        }

        for (auto& key : removeKeys)
        {
            m_downloadTasks.erase(key);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

}  // namespace download
