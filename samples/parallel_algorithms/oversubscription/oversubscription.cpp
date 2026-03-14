#include "MeasureTime.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <cmath>
#include <thread>

constexpr std::size_t kIterations = 100'000'000;

void Worker(std::size_t begin, std::size_t end)
{
	double x = 0.0;

	for (std::size_t i = begin; i < end; ++i)
	{
		x += std::sin(i);
	}

	volatile double sink = x;
}

void Run(std::size_t threads)
{
	std::vector<std::jthread> workers;
	workers.reserve(threads);

	const std::size_t blockSize = kIterations / threads;

	for (std::size_t t = 0; t < threads; ++t)
	{
		const std::size_t begin = t * blockSize;
		const std::size_t end = (t + 1 == threads) ? kIterations : (t + 1) * blockSize;

		workers.emplace_back([begin, end] {
			Worker(begin, end);
		});
	}
}

int main()
{
	const unsigned hw = std::thread::hardware_concurrency();

	for (std::size_t t = 1; t <= hw * 2; ++t)
	{
		MeasureTime(std::to_string(t) + " thread(s)", Run, t);
	}
}