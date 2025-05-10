#pragma once
#include <mutex>
#include <condition_variable>

class ManualResetEvent
{
public:
	void Set()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_isSet = true;
		m_cond.notify_all();
	}

	void Wait()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait(lock, [this] { return m_isSet; });
	}

	void Reset()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_isSet = false;
	}

private:
	std::mutex m_mutex;
	std::condition_variable m_cond;
	bool m_isSet = false;
};
