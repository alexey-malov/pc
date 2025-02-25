#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <syncstream>
#include <thread>

using namespace std::literals;

std::jthread MakePhilosopher(int index, std::mutex& leftFork, std::mutex& rightFork)
{
	return std::jthread([&leftFork, &rightFork, index] {
		int eatTimes = 0;
		for (;;)
		{
			{
				std::scoped_lock lk{ leftFork, rightFork };
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
		philosophers[i] = MakePhilosopher(i, forks[i], forks[(i + 1) % numPhilosophers]);
}
