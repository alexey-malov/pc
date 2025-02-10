#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

template <typename T>
std::jthread SortVector(std::vector<T>& values)
{
	return std::jthread{ [&values] {
		std::ranges::sort(values);
	} };
}

int main()
{
	std::vector<int> numbers{ 10, 2, -5, 3, 17, 5 };
	std::vector<std::string> strings{ "one", "two", "three", "four", "five" };
	{
		auto t1 = SortVector(numbers);
		auto t2 = SortVector(strings);
	} // При выходе из блока массивы будут отсортированы
	assert(std::ranges::is_sorted(numbers));
	assert(std::ranges::is_sorted(strings));
}
