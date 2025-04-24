#include <functional>
#include <generator>
#include <iostream>
#include <optional>

namespace
{
std::generator<int> Counter()
{
	for (int i = 0; i < 10; ++i)
	{
		co_yield i;
	}
}

// Функция, которая передаёт результат вычислений в callback.
void Counter(const std::function<void(int)>& callback)
{
	for (int i = 0; i < 10; ++i)
	{
		callback(i);
	}
}

std::generator<int> CountToThree()
{
	co_yield 1;
	co_yield 2;
	co_yield 3;
}

} // namespace

int main()
{
	Counter([](int i) {
		std::cout << i << std::endl;
	});

	for (auto number : Counter())
	{
		std::cout << number << std::endl;
	}

	auto gen = CountToThree();
	for (auto it = gen.begin(); it != gen.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}
