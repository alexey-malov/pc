#include "MeasureTime.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <vector>

long long Sequential(const std::vector<int>& v)
{
	long long sum = 0;
	for (int x : v)
	{
		sum += x;
	}
	return sum;
}

long long ParallelBad(const std::vector<int>& v)
{
	std::atomic<long long> sum = 0;

	std::for_each(std::execution::par, v.begin(), v.end(),
		[&](int x) {
			sum.fetch_add(x, std::memory_order_relaxed);
		});

	return sum.load();
}

int main()
{
	std::vector<int> v(100'000'000, 1);

	MeasureTime("sequential", Sequential, v);
	MeasureTime("parallel bad", ParallelBad, v);
}