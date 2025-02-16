#include <iostream>
#include <thread>

volatile int counter = 0; // Бесполезно надеемся на чудо, делая переменную волатильной

int main()
{
	std::jthread t1{ [] {
		for (int i = 0; i < 1'000'000; ++i)
			counter += 1;
	} };
	std::jthread t2{ [] {
		for (int i = 0; i < 1'000'000; ++i)
			counter -= 1;
	} };
	t1.join();
	t2.join();
	std::cout << counter << std::endl;
}
