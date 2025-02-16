#include <iostream>
#include <thread>

void Increment(int* n)
{
	++(*n);
}

int main()
{
	int n = 0;
	std::cout << "n = " << n << std::endl;

	std::jthread th1{ [&n] { n = 1; } };
	th1.join();

	std::cout << "Now n = " << n << std::endl;

	std::jthread th2{ Increment, &n };
	th2.join();

	std::cout << "Now n = " << n << std::endl;
}
