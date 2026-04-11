#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <syncstream>
#include <thread>

int main()
{
	using namespace std::chrono_literals;
	std::string message;
	std::atomic<bool> ready{ false };

	std::thread worker([&] {
		message = "Hello from worker thread"; // Обычная, неатомарная запись
		ready.store(true, std::memory_order_release); // Публикация результата
		std::this_thread::sleep_for(1s); // продолжаем работу
		std::osyncstream(std::cout) << "End of worker thread\n";
	});

	while (!ready.load(std::memory_order_acquire))
	{ // Ждём публикации
		std::this_thread::yield(); // Пассивное ожидание
	}

	std::osyncstream(std::cout) << message << '\n'; // Безопасное неатомарное чтение

	worker.join(); // Поток завершится через секунду, после публикации
}