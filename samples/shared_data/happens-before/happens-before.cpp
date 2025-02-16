#include <iostream>
#include <thread>

int main()
{
	std::jthread t{ [] {
		std::cout << "A";
		std::jthread a{ [] { std::cout << "B"; } };
		std::cout << "C";
		std::jthread b{ [] { std::cout << "D"; } };
		std::cout << "E";
		a.join();
		std::cout << "F";
	} };
	std::cout << "G";
	t.join();
	std::cout << "H";
}
