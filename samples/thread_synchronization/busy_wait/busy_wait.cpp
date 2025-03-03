#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;

auto WaitForFlag(bool& condition, std::mutex& m)
{
	for (long long checkCount = 1;; ++checkCount)
	{
		std::lock_guard lk{ m };
		if (condition)
			return checkCount;
	}
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
