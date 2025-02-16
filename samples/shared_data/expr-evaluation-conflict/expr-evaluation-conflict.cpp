#include <iostream>

int main()
{
	int a = 0;
	auto incrementA = [&a] {
		++a;
	};

	a = 5; // Запись в переменную `a`
	incrementA(); // Запись в переменную `a`
	std::cout << (a + 1) << std::endl; // Чтение переменной `a`

	int b = 4;
	std::cout << (b + 1) << ' ' << (b - 1); // Два чтения из переменной b
}
