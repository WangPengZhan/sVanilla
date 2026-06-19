#include "ThreadPool.h"
#include "ThreadPoolLog.h"

ThreadPool::ThreadPool(size_t threads)
    : m_stop(false)
    , m_numThreads(threads)
{
    if (threads == 0)
    {
        return;
    }

    for (size_t i = 0; i < threads; ++i)
    {
        m_workers.emplace_back(std::thread(&ThreadPool::workLoop, this));
        THEADPOOL_LOG_INFO("ThreadPool worker {} created", i);
    }

    size_t thread_count = 0;
    for (std::thread& worker : m_workers)
    {
        m_idMap[worker.get_id()] = thread_count;
        thread_count++;
    }
}

ThreadPool::~ThreadPool()
{
    stop();
}

int ThreadPool::threadNumber(std::thread::id id)
{
    if (m_idMap.find(id) != m_idMap.end())
    {
        return m_idMap[id];
    }

    return -1;
}

size_t ThreadPool::numThreads()
{
    return m_numThreads;
}

void ThreadPool::stop()
{
    m_stop = true;
    {
        std::lock_guard lk(m_tasksMutex);
        m_tasks.clear();
    }
    m_condition.notify_all();
    for (std::thread& worker : m_workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void ThreadPool::drop()
{
    {
        std::lock_guard<std::mutex> lock(m_tasksMutex);
        m_tasks.clear();
    }
}

void ThreadPool::workLoop()
{
    THEADPOOL_LOG_INFO("ThreadPool worker start, thread hash: {}", std::hash<std::thread::id>{}(std::this_thread::get_id()));
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_tasksMutex);
            m_condition.wait(lock, [&] {
                return m_stop || !m_tasks.empty();
            });
            if (m_stop && m_tasks.empty())
            {
                THEADPOOL_LOG_INFO("ThreadPool worker exit, thread hash: {}", std::hash<std::thread::id>{}(std::this_thread::get_id()));
                return;
            }
            task = std::move(m_tasks.front());
            m_tasks.pop_front();
        }
        task();
    }
}
