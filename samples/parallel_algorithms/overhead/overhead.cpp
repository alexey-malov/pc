#include "MeasureTime.h"
#include <algorithm>
#include <chrono>
#include <execution>
#include <future>
#include <iostream>
#include <random>
#include <vector>

int Sequential(const std::vector<int>& v)
{
	int sum = 0;
	for (int x : v)
	{
		sum += x * x;
	}
	return sum;
}

int ParallelBad(const std::vector<int>& v)
{
	std::vector<std::future<int>> futures;
	futures.reserve(v.size());

	for (int x : v)
	{
		futures.push_back(std::async(std::launch::async, [x] {
			return x * x;
		}));
	}

	int sum = 0;
	for (auto& f : futures)
	{
		sum += f.get();
	}
	return sum;
}

int main()
{
	std::vector<int> v(10000, 2);

	MeasureTime("sequential", Sequential, v);
	MeasureTime("parallel bad", ParallelBad, v);
}