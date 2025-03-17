#include <iostream>
#include <mutex>
#include <thread>

int main()
{
	std::once_flag flag;
	std::jthread t1{ [&flag] {
		std::call_once(flag, [] { std::cout << "T1\n"; });
	} };
	std::jthread t2{ [&flag] {
		std::call_once(flag, [] { std::cout << "T2\n"; });
	} };
	std::jthread t3{ [&flag] {
		std::call_once(flag, [] { std::cout << "T3\n"; });
	} };
}
