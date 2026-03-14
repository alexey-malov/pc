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

long long ParallelGood(const std::vector<int>& v)
{
	return std::reduce(std::execution::par, v.begin(), v.end(), 0LL);
}

int main()
{
	std::vector<int> v(100'000'000, 1);

	MeasureTime("sequential", Sequential, v);
	MeasureTime("parallel good", ParallelGood, v);
}