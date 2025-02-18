#include <iostream>
#include <mutex>
#include <thread>

std::mutex m1;
std::mutex m2;
int x = 0;

int main()
{
	std::jthread t1{ [] { // A
		std::lock_guard lk{ m1 }; // B
		++x; // C
		// D
	} }; // E
	std::jthread t2{ [] { // F
		std::lock_guard lk{ m2 }; // G
		++x; // H
		// I
	} }; // J
	std::lock_guard lk{ m1 }; // K
	std::cout << x << std::endl; // L
} // M
