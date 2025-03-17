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
		std::call_once(m_initFlag, [this] {
			m_data = new Data();
		});
		m_data->UseData();
	}

private:
	mutable const Data* m_data = nullptr;
	mutable std::once_flag m_initFlag;
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
	MeasureTime("DCL (wrong) version", TestSingleThreaded);
}
