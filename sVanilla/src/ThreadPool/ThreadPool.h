#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <map>
#include <type_traits>
#include <iostream>

#include "ThreadPool/Task.h"

class ThreadPool
{
public:
	ThreadPool(size_t num);
	~ThreadPool();

	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)
		-> std::future<typename std::invoke_result<F, Args...>::type>;
	template<typename T, typename std::enable_if<
		std::is_base_of<Task, typename std::remove_cv<T>::type>::value,
		int>::type = 0>
	std::future<void> enqueue(T* pTask);

	static ThreadPool& instance();

	int threadNumber(std::thread::id id);
	size_t numThreads();

	void stop();
	void drop();

	void workLoop();

private:
	std::vector<std::thread> m_workers;
	std::list<std::function<void()>> m_tasks;
	std::mutex m_tasksMutex;
	std::condition_variable m_condition;
	volatile bool m_stop;
	std::unordered_map<std::thread::id, int> m_idMap;
	size_t m_numThreads;
};

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
	-> std::future<typename std::invoke_result<F, Args...>::type>
{
	using return_type = typename std::invoke_result<F, Args...>::type;

	auto task = std::make_shared<std::packaged_task<return_type()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...));

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(m_tasksMutex);
		if (m_stop)
		{
			throw std::runtime_error("enqueue on stopped ThreadPool");
		}
		m_tasks.emplace_back([task]() { (*task)(); });
	}
	m_condition.notify_one();

	return res;
}

template<typename T, typename std::enable_if<
	std::is_base_of<Task, typename std::remove_cv<T>::type>::value, 
	int>::type = 0>
std::future<void> ThreadPool::enqueue(T* pTask)
{
	auto task = [pTask]() {
		pTask->Run();
		pTask->Destory();
	};
	return enqueue(task);
}
