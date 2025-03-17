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
		std::osyncstream(std::cout) << "Data::Data()\n";
	}

	void UseData() const
	{
		std::osyncstream(std::cout) << "Data::UseData()\n";
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
		std::osyncstream(std::cout) << "Widget::Foo()\n";
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

int main()
{
	Widget widget;

	const unsigned numThreads = 8;
	std::latch latch{ numThreads };
	std::vector<std::jthread> threads;

	for (unsigned i = 0; i < numThreads; ++i)
	{
		threads.emplace_back([&latch, &widget] {
			latch.arrive_and_wait();
			widget.Foo(); });
	};
}
