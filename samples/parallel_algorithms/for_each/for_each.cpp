#include "MeasureTime.h"
#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using IntMatrix = std::vector<std::vector<int>>;

IntMatrix GenerateSequential(size_t numRows, size_t numColumns)
{
	IntMatrix mat(numRows);
	for (auto& row : mat)
	{
		row.reserve(numColumns);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution dist(0, 1'000'000);

		for (size_t c = 0; c < numColumns; ++c)
		{
			row.push_back(dist(gen));
		}
	}
	return mat;
}

IntMatrix GenerateParallel(size_t numRows, size_t numColumns)
{
	IntMatrix m(numRows);
	std::for_each(std::execution::par, m.begin(), m.end(), [numColumns](auto& row) {
		row.reserve(numColumns);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution dist(0, 1'000'000);

		for (size_t c = 0; c < numColumns; ++c)
		{
			row.push_back(dist(gen));
		}
	});
	return m;
}

int main()
{
	MeasureTime("sequential", GenerateSequential, 10'000, 100'000);
	MeasureTime("parallel", GenerateParallel, 10'000, 100'000);
}
