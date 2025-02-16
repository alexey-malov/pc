#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <syncstream>
#include <thread>

using Clock = std::chrono::high_resolution_clock;
using Seconds = std::chrono::duration<double>;
using namespace std::chrono;

class TSCounter
{
public:
	void Increment()
	{
		std::unique_lock lk{ m_mutex };
		++m_value;
	}

	void Decrement()
	{
		std::unique_lock lk{ m_mutex };
		--m_value;
	}

	int Get() const
	{
		std::shared_lock lk{ m_mutex };
		return m_value;
	}

	void Reset()
	{
		std::unique_lock lk{ m_mutex };
		m_value = 0;
	}

private:
	int m_value = 0;
	mutable std::shared_mutex m_mutex;
};

int main()
{
	TSCounter counter;
	std::thread t1{ [&counter] {
		for (int i = 0; i < 1'000'000; ++i)
		{
			counter.Increment();
		}
	} };
	std::thread t2{ [&counter] {
		for (int i = 0; i < 100; ++i)
		{
			std::osyncstream(std::cout) << "t2:" << counter.Get() << std::endl;
		}
	} };
	for (int i = 0; i < 100; ++i)
	{
		std::osyncstream(std::cout) << "main:" << counter.Get() << std::endl;
	}
	t1.join();
	t2.join();
	std::cout << counter.Get() << std::endl;
}
