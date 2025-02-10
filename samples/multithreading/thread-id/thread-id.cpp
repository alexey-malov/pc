#include <iostream>
#include <memory>
#include <string>
#include <thread>

int main()
{
	std::cout << "This thread id: " << std::this_thread::get_id() << "\n";
	std::jthread bgThread{ [] {
		std::cout << "In bg thread. Id is " << std::this_thread::get_id() << "\n";
	} };
	std::cout << "Bg thread id: " << bgThread.get_id() << "\n";
	bgThread.join();
	std::cout << "After join bg thread id: " << bgThread.get_id() << "\n";
}