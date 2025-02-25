#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <syncstream>
#include <thread>

using namespace std::literals;

std::jthread MakePhilosopher(int index, std::mutex& fork1, std::mutex& fork2)
{
	return std::jthread([&fork1, &fork2, index] {
		int eatTimes = 0;
		for (;;)
		{
			{
				std::lock_guard lk1{ fork1 };
				std::lock_guard lk2{ fork2 };
				++eatTimes;
				std::osyncstream(std::cout) << "Philosopher #" << index
											<< " is eating #" << eatTimes << "\n";
			}
			std::osyncstream(std::cout) << "Philosopher #" << index << " is thinking\n";
		}
	});
}

int main()
{
	constexpr int numPhilosophers = 5;
	std::array<std::mutex, numPhilosophers> forks{};
	std::array<std::jthread, numPhilosophers> philosophers;
	for (int i = 0; i < numPhilosophers; ++i)
	{
		int j = (i + 1) % numPhilosophers;
		philosophers[i] = MakePhilosopher(i, forks[std::min(i, j)], forks[std::max(i, j)]);
	}
}
