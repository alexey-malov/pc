#include "MeasureTime.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <array>
#include <string>

constexpr std::size_t kThreads = 8;
constexpr std::size_t kIterations = 500'000'000;

alignas(std::hardware_destructive_interference_size)
	std::array<volatile std::uint32_t, kThreads> gCounters{};

void RunFalseSharing(std::size_t threads)
{
	for (auto& x : gCounters)
		x = 0;

	std::vector<std::jthread> workers;
	workers.reserve(threads);

	for (std::size_t t = 0; t < threads; ++t)
	{
		workers.emplace_back([t] {
			for (std::size_t i = 0; i < kIterations; ++i)
			{
				++gCounters[t];
			}
		});
	}
}

int main()
{
	for (std::size_t t = 1; t <= kThreads; ++t)
	{
		MeasureTime(std::to_string(t) + " thread(s)", RunFalseSharing, t);
	}
}