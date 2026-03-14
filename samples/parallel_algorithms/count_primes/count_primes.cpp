#include "MeasureTime.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <string>
#include <vector>

bool IsPrime(size_t n)
{
	if (n < 2)
		return false;

	for (size_t d = 2; d < n; ++d)
	{
		if (n % d == 0)
			return false;
	}

	return true;
}

size_t CountPrimesInRange(size_t begin, size_t end)
{
	size_t primes = 0;
	for (size_t x = begin; x < end; ++x)
	{
		if (IsPrime(x))
		{
			++primes;
		}
	}
	return primes;
}

size_t CountPrimesParallel(size_t n, size_t threads)
{
	std::atomic<size_t> total = 0;
	std::vector<std::jthread> workers;
	workers.reserve(threads);

	const size_t blockSize = n / threads;

	for (size_t t = 0; t < threads; ++t)
	{
		const size_t begin = t * blockSize;
		const size_t end = (t + 1 == threads) ? (n + 1) : ((t + 1) * blockSize);

		workers.emplace_back([begin, end, &total] {
			total += CountPrimesInRange(begin, end);
		});
	}

	return total.load();
}

int main()
{
	constexpr size_t N = 200'000;

	for (int t = 1; t <= 8; ++t)
	{
		MeasureTime("count primes. threads=" + std::to_string(t), CountPrimesParallel, N, t);
	}
}