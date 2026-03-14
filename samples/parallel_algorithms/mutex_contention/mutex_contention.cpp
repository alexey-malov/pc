#include "MeasureTime.h"
#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <mutex>
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
	long long sum = 0;
	std::mutex m;

	std::for_each(std::execution::par, v.begin(), v.end(),
		[&](int x) {
			std::lock_guard<std::mutex> lock(m);
			sum += x;
		});

	return sum;
}

int main()
{
	std::vector<int> v(100'000'000, 1);

	MeasureTime("sequential", Sequential, v);
	MeasureTime("parallel bad", ParallelBad, v);
}