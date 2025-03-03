#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;

auto WaitForFlag(bool& condition, std::mutex& m)
{
	long long checkCount = 1;
	for (std::unique_lock lk{ m }; // Захватываем мьютекс
		 !condition; // Проверяем наступление условия
		 lk.lock(), // Захватываем мьютекс перед следующей проверой условия
		 ++checkCount)
	{
		lk.unlock(); // Освобождаем мьютекс
		std::this_thread::sleep_for(100ms); // Спим, чтобы не тратить время CPU
	}
	return checkCount;
}

int main()
{
	bool dataIsReady = false;
	std::mutex dataMutex;
	std::string data;

	std::jthread consumer{ [&dataIsReady, &dataMutex, &data] {
		std::osyncstream(std::cout) << "Consumer is waiting for data\n";
		auto count = WaitForFlag(dataIsReady, dataMutex);
		std::osyncstream(std::cout) << "Consumer checked flag " << count << " times\n";
		std::osyncstream(std::cout) << data << "\n";
	} };

	std::jthread producer{ [&dataIsReady, &dataMutex, &data] {
		std::cout << "Producer is preparing data\n";
		std::this_thread::sleep_for(1s);
		data = "Hello";
		std::cout << "Producer has prepared data\n";
		std::lock_guard lk{ dataMutex };
		dataIsReady = true;
		std::cout << "Producer has set the dataIsReady flag\n";
	} };
}
