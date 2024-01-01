#include <vector>

#include "DownloadStatusThread.h"
#include "AbstractDownloader.h"

namespace download
{

DownloadStatusThread::DownloadStatusThread() : m_running(true), m_thread(&DownloadStatusThread::downloadThread, this)
{
}

DownloadStatusThread::~DownloadStatusThread()
{
    stop();
    m_thread.join();
}

void DownloadStatusThread::stop()
{
    m_running.store(false);
}

void DownloadStatusThread::downloadThread()
{
    while (m_running.load())
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> removeKeys;
        for (auto& [key, value] : m_downloadTasks)
        {
            switch (value->status())
            {
            case AbstractDownloader::Ready: {
                value->start();
                break;
            }
            case AbstractDownloader::Downloading: {
                value->downloadStatus();
                break;
            }
            case AbstractDownloader::Paused: {
                value->pause();
                removeKeys.push_back(key);
                break;
            }
            case AbstractDownloader::Stopped: {
                value->stop();
                removeKeys.push_back(key);
                break;
            }
            case AbstractDownloader::Finished:
            case AbstractDownloader::Error: {
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
