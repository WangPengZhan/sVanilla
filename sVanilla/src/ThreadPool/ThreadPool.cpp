#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threads) : m_stop(false), m_numThreads(threads)
{
	if (threads == 0)
	{
		return;
	}

	for (size_t i = 0; i < threads; ++i)
	{
		m_workers.emplace_back(std::thread(&ThreadPool::workLoop, this));
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
	for (std::thread& worker : m_workers)
	{
		worker.join();
	}
}

ThreadPool& ThreadPool::instance()
{
    static ThreadPool threadPool(std::thread::hardware_concurrency());
    return threadPool;
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
	m_condition.notify_all();
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
	while (true)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(m_tasksMutex);
			m_condition.wait(lock,
				[&] { return m_stop || !m_tasks.empty(); });
			if (m_stop && m_tasks.empty())
			{
				return;
			}
			task = std::move(m_tasks.front());
			m_tasks.pop_front();
		}
		task();
	}
}
