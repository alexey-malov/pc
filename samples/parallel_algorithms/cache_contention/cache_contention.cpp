#include "MeasureTime.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <vector>
#include <string>

void Worker(const std::vector<int>& data, size_t iterations)
{
	long long sum = 0;

	for (size_t iter = 0; iter < iterations; ++iter)
	{
		sum = std::accumulate(data.begin(), data.end(), sum);
	}

	volatile long long sink = sum;
}

void Run(size_t threads, size_t size, size_t iterations)
{
	std::vector<std::vector<int>> data(threads, std::vector<int>(size, 1));

	std::vector<std::jthread> workers;
	workers.reserve(threads);

	for (size_t t = 0; t < threads; ++t)
	{
		workers.emplace_back([&data, t, iterations] {
			Worker(data[t], iterations);
		});
	}
}

int main()
{
	for (size_t t = 1; t <= 8; ++t)
	{
		MeasureTime(std::to_string(t) + " thread(s) (1K)", Run, t, 1024, 10'000'000);
	}
	for (size_t t = 1; t <= 8; ++t)
	{
		MeasureTime(std::to_string(t) + " thread(s) (4M)", Run, t, 4 * 1024 * 1024, 2000);
	}
}