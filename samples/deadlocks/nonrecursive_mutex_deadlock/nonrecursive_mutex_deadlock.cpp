#include <iostream>
#include <mutex>

int main()
{
	std::mutex m;

	std::lock_guard lk1{ m };
	std::cout << "1st lock\n";

	std::lock_guard lk2{ m };
	std::cout << "2nd lock\n";
}
