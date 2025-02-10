#include "MeasureTime.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <thread>
#include <vector>

namespace detail
{
template <typename It, typename T>
struct AccumulateBlock
{
	void operator()(It begin, It end, T& result)
	{
		result = std::accumulate(begin, end, result);
	}
};
} // namespace detail

template <typename Iterator, typename T>
T ParallelAccumulate(Iterator begin, Iterator end, T init, size_t minPerThread = 10)
{
	const size_t size = std::distance(begin, end);
	if (!size)
		return init;

	const size_t maxThreads = (size + minPerThread - 1) / minPerThread;

	const size_t hwThreads = std::thread::hardware_concurrency();
	const auto numThreads = std::min(hwThreads != 0 ? hwThreads : 2u, maxThreads);

	const auto blockSize = size / numThreads;

	std::vector<T> results(numThreads);
	{
		std::vector<std::jthread> threads(numThreads);

		auto blockStart = begin;
		for (size_t i = 0; i < numThreads - 1; ++i)
		{
			auto blockEnd = blockStart;
			std::advance(blockEnd, blockSize);
			threads[i] = std::jthread{
				detail::AccumulateBlock<Iterator, T>(),
				blockStart, blockEnd, std::ref(results[i])
			};
			blockStart = blockEnd;
		}
		detail::AccumulateBlock<Iterator, T>()(blockStart, end, results[numThreads - 1]);
	}
	return std::accumulate(results.begin(), results.end(), init);
}

int main()
{
	constexpr size_t size = 100'000'000;
	std::vector<unsigned> numbers;
	numbers.reserve(size);
	std::mt19937 gen;
	std::generate_n(std::back_inserter(numbers), size, [&gen] {
		return std::uniform_int_distribution<unsigned>(0, 10)(gen);
	});
	auto sum = ParallelAccumulate(numbers.begin(), numbers.end(), 0u);
	assert(sum == std::accumulate(numbers.begin(), numbers.end(), 0u));

	MeasureTime("Single-threaded", [&numbers] {
		volatile unsigned result = 0;
		result = std::accumulate(numbers.begin(), numbers.end(), 0u);
	});
	MeasureTime("Multi-threaded", [&numbers] {
		volatile int result = 0;
		result = ParallelAccumulate(numbers.begin(), numbers.end(), 0u);
	});
}
