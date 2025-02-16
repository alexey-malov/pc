#include <iostream>
#include <thread>

int main()
{
	int counter = 0;
	std::jthread t1{ [&counter] {
		for (int i = 0; i < 1'000'000; ++i)
			++counter;
	} };
	std::jthread t2{ [&counter] {
		for (int i = 0; i < 1'000'000; ++i)
			--counter;
	} };
	t1.join();
	t2.join();
	std::cout << counter << std::endl;
}
