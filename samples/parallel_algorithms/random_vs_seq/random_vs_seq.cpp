#include "MeasureTime.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <array>

constexpr std::size_t kCacheLineSize = std::hardware_destructive_interference_size;
constexpr std::size_t kIterations = 2'000'000'000;

alignas(std::hardware_destructive_interference_size)
	std::array<std::uint8_t, 64 * 1024 * 1024> gData{};

std::uint64_t SumCacheLine(std::size_t x, std::size_t y)
{
	std::uint64_t sum = 0;
	std::size_t pos = 0;

	for (std::size_t i = 0; i < kIterations; ++i)
	{
		sum += gData[pos];
		pos = (pos * x + y) % gData.size();
	}

	return sum;
}

int main()
{
	volatile std::size_t seqX = 0;
	volatile std::size_t seqY = 1;
	volatile std::size_t randX = 19553;
	volatile std::size_t randY = 6079;
	MeasureTime("sequential", SumCacheLine, seqX, seqY);
	MeasureTime("pseudo-random", SumCacheLine, randX, randY);
}