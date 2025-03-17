#include "MeasureTime.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <latch>
#include <memory>
#include <mutex>
#include <syncstream>
#include <thread>
#include <vector>

class SpinMutex
{
public:
	void lock() noexcept
	{
		while (m_flag.test_and_set())
		{
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
			m_flag.wait(true);
#endif
		}
	}

	bool try_lock() noexcept
	{
		return !m_flag.test_and_set();
	}

	void unlock() noexcept
	{
		m_flag.clear();
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
		m_flag.notify_one();
#endif
	}

private:
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

class Data
{
public:
	Data()
	{
	}

	void UseData() const
	{
	}
};

class Widget
{
public:
	Widget() = default;
	Widget(const Widget&) = delete;
	Widget& operator=(const Widget&) = delete;
	~Widget()
	{
		delete m_data;
	}

	void Foo() const
	{
		if (std::lock_guard lk{ m_mutex }; !m_data)
		{
			m_data = new Data();
		}
		m_data->UseData();
	}

private:
	mutable const Data* m_data = nullptr;
	mutable SpinMutex m_mutex;
};

void TestSingleThreaded()
{
	Widget widget;
	const unsigned numThreads = 8;
	std::vector<std::jthread> threads;
	for (int t = 0; t < numThreads; ++t)
	{
		threads.emplace_back([&widget] {
			for (int i = 0; i < 10'000'000; ++i)
				widget.Foo();
		});
	}
}

int main()
{
	MeasureTime("Spin mutex", TestSingleThreaded);
}
