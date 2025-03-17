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
	mutable std::mutex m_mutex;
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
	MeasureTime("ThreadSafe", TestSingleThreaded);
}
