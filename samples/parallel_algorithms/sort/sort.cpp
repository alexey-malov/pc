#include "MeasureTime.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <execution>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

using Numbers = std::vector<int>;

Numbers Generate(size_t size)
{
	Numbers numbers;
	numbers.reserve(size);
	std::mt19937 gen;
	std::uniform_int_distribution dist(0, 10'000'000);
	std::generate_n(std::back_inserter(numbers), size, [&dist, &gen] {
		return dist(gen);
	});
	return numbers;
}

void SortSeq(Numbers& numbers)
{
	std::sort(numbers.begin(), numbers.end());
}

void SortPar(Numbers& numbers)
{
	std::sort(std::execution::par, numbers.begin(), numbers.end());
}

void SortUnseq(Numbers& numbers)
{
	std::sort(std::execution::unseq, numbers.begin(), numbers.end());
}

void SortParUnseq(Numbers& numbers)
{
	std::sort(std::execution::par_unseq, numbers.begin(), numbers.end());
}

int main()
{
	const auto numbers = Generate(100'000'000);

	auto n = numbers;
	MeasureTime("seq", SortSeq, n);

	n = numbers;
	MeasureTime("par", SortPar, n);

	n = numbers;
	MeasureTime("unseq", SortUnseq, n);

	n = numbers;
	MeasureTime("par_unseq", SortParUnseq, n);
}
