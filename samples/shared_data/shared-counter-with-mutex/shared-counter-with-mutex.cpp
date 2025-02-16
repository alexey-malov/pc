#include <iostream>
#include <mutex>
#include <thread>

int main()
{
	std::mutex m;
	int counter = 0;
	std::jthread t1{ [&counter, &m] {
		for (int i = 0; i < 1'000'000; ++i)
		{
			m.lock();
			++counter;
			m.unlock();
		}
	} };
	std::jthread t2{ [&counter, &m] {
		for (int i = 0; i < 1'000'000; ++i)
		{
			std::lock_guard lk{ m }; // Конструктор lock-guard вызывает lock
			--counter;
		}
	} };
	t1.join();
	t2.join();
	std::cout << counter << std::endl;
}
