#include "MeasureTime.h"
#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <vector>

void Sequential(std::vector<int>& v)
{
	for (int i = 0; i < 10000;++i)
	{
		std::for_each(v.begin(), v.end(), [](int& x) {
			x *= 2;
		});
	}
}

void Parallel(std::vector<int>& v)
{
	for (int i = 0; i < 10000; ++i)
	{
		std::for_each(std::execution::par, v.begin(), v.end(), [](int& x) {
			x *= 2;
		});
	}
}

int main()
{
	std::vector<int> v(10000); // очень маленький массив
	MeasureTime("sequential", Sequential, v);
	MeasureTime("parallel", Parallel, v);
}
