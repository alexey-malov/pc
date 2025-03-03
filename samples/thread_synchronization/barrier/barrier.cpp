#include <barrier>
#include <chrono>
#include <iostream>
#include <latch>
#include <mutex>
#include <queue>
#include <syncstream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

void OnCompletion() noexcept
{
	std::osyncstream(std::cout) << "---- " << std::this_thread::get_id() << " ----\n";
}

int main()
{
	std::string workerNames[] = { "Alice", "Bob", "Caroline" };

	std::barrier syncPoint{ std::ssize(workerNames), &OnCompletion };

	auto work = [&syncPoint](std::string workerName) {
		std::osyncstream(std::cout) << workerName << " thread id: " << std::this_thread::get_id() << "\n";
		std::osyncstream(std::cout) << workerName << " is preparing a hot dog\n";
		std::this_thread::sleep_for(1s);
		std::osyncstream(std::cout) << workerName << " has prepared the hot dog\n";

		syncPoint.arrive_and_wait(); // Ждём остальные потоки

		std::osyncstream(std::cout) << workerName << " is eating the hot dog\n";
		std::this_thread::sleep_for(1s);
		std::osyncstream(std::cout) << workerName << " has eaten the hot dog\n";

		syncPoint.arrive_and_wait(); // Ждём остальные потоки
	};

	std::vector<std::jthread> workers;
	for (const auto& workerName : workerNames)
	{
		workers.emplace_back(work, std::string(workerName));
	}
}