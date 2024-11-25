#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue
{
private:
	std::queue<T> queue;
	mutable std::mutex mtx;
	std::condition_variable cv;

public:
	void push(const T &item)
	{
		std::lock_guard<std::mutex> lock(mtx);
		queue.push(item);
		cv.notify_one();
	}

	bool try_pop(T &item)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (queue.empty())
		{
			return false;
		}
		item = queue.front();
		queue.pop();
		return true;
	}

	void wait_and_pop(T &item)
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this]()
		{ return !queue.empty(); });
		item = queue.front();
		queue.pop();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(mtx);
		return queue.empty();
	}
};
