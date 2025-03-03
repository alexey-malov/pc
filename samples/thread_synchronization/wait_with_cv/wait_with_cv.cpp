#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;

int main()
{
	bool dataIsReady = false;
	std::mutex dataMutex;
	std::string data;
	std::condition_variable readyCV; // Условная переменная, сообщающая о готовности данных

	std::jthread consumer{ [&dataIsReady, &dataMutex, &readyCV, &data] {
		std::osyncstream(std::cout) << "Consumer is waiting for data\n";

		long long checkCounter = 1;
		std::unique_lock lk{ dataMutex };
		readyCV.wait(lk, [&dataIsReady, &checkCounter] { // Функция-предикат
			++checkCounter;
			return dataIsReady; // Ожидание продолжается, пока предикат не вернёт true
		});
		// При выходе из wait мьютекс будет заблокирован текущим потоком
		std::osyncstream(std::cout) << "Consumer checked flag " << checkCounter << " times\n";
		std::osyncstream(std::cout) << data << "\n";
	} };

	std::jthread producer{ [&dataIsReady, &dataMutex, &data, &readyCV] {
		std::cout << "Producer is preparing data\n";
		std::this_thread::sleep_for(1s);
		data = "Hello";
		std::cout << "Producer has prepared data\n";
		{
			std::lock_guard lk{ dataMutex };
			dataIsReady = true;
		}
		std::cout << "Producer has set the dataIsReady flag\n";
		readyCV.notify_one(); // Уведомляем ожидающий поток о готовности
	} };
}
