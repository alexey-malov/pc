#include <iostream>
#include <thread>

int main()
{
	int counter = 0;
	std::jthread t1{ [&counter] {
		for (int i = 0; i < 1'000'000; ++i)
			++counter;
	} };
	t1.join();

	// Вызов t1.join() happens-before (происходит до) запуска потока t2
	std::jthread t2{ [&counter] {
		for (int i = 0; i < 1'000'000; ++i)
			--counter;
	} };
	t2.join();

	// Вызов t2.join() happens-before (происходит до) чтения counter
	std::cout << counter << std::endl;
}
