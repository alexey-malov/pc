#include "MeasureTime.h"
#include <array>

constexpr std::size_t kCacheLineSize = std::hardware_destructive_interference_size;
constexpr std::size_t kIterations = 2'000'000'000;

alignas(std::hardware_destructive_interference_size)
	std::array<std::uint8_t, std::hardware_destructive_interference_size> gData{};

std::uint64_t SumCacheLine(std::size_t x, std::size_t y)
{
	std::uint64_t sum = 0;
	std::size_t pos = 0;

	for (std::size_t i = 0; i < kIterations; ++i)
	{
		sum += gData[pos];
		pos = (pos * x + y) % kCacheLineSize;
	}

	return sum;
}

int main()
{
	// volatile здесь нужен, чтобы помешать копмилятору посчитать результат во время компиляции
	volatile std::size_t seqX = 0;
	volatile std::size_t seqY = 1;
	volatile std::size_t randX = 13;
	volatile std::size_t randY = 27;
	MeasureTime("sequential", SumCacheLine, seqX, seqY);
	MeasureTime("pseudo-random", SumCacheLine, randX, randY);
}