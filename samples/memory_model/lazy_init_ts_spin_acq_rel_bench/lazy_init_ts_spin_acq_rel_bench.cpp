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
#include <cassert>

class SpinMutex
{
public:
	SpinMutex() = default;

	~SpinMutex()
	{
		assert(m_flag.load(std::memory_order::relaxed) == false);
	}
	void lock() noexcept
	{
		bool current;
		do
		{
			current = false;
		} while (!m_flag.compare_exchange_weak(current, true, std::memory_order::acquire));
	}

	bool try_lock() noexcept
	{
		
		bool current = false;
		return m_flag.compare_exchange_strong(current, true, std::memory_order::acquire);
	}

	void unlock() noexcept
	{
		m_flag.store(false, std::memory_order::release);
	}

private:
	std::atomic_bool m_flag = false;
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
