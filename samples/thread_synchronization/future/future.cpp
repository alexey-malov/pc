#include <chrono>
#include <future>
#include <iostream>
#include <stdexcept>
#include <syncstream>

using namespace std::chrono_literals;

int Add(int x, int y)
{
	std::osyncstream(std::cout) << "Adding. Thread id: " << std::this_thread::get_id() << "\n";
	std::this_thread::sleep_for(1s);
	int result = x + y;
	std::osyncstream(std::cout) << "Added, result=" << result << "\n";
	return result;
}

int Multiply(int x, int y)
{
	std::osyncstream(std::cout) << "Multiplying. Thread id: " << std::this_thread::get_id() << "\n";
	std::this_thread::sleep_for(2s);
	int result = x * y;
	std::osyncstream(std::cout) << "Multiplied, result: " << result << "\n";
	return result;
}

int main()
{
	std::cout << "Main thread id: " << std::this_thread::get_id() << "\n";
	auto f1 = std::async(std::launch::async, Add, 5, 2);
	auto f2 = std::async(std::launch::async, Multiply, 3, 10);
	auto f3 = std::async(std::launch::deferred, Add, 2, 3);
	std::cout << "---\n";
	auto f4 = std::async(std::launch::async, Add, f1.get(), f2.get());
	std::cout << "---\n";
	std::cout << f4.get() << "\n";
	std::cout << "---\n";
	std::cout << f3.get() << "\n";

	auto f5 = std::async(std::launch::async, []() -> int {
		throw std::runtime_error("Oops");
	});
	std::cout << "!!!\n";
	try
	{
		std::cout << f5.get() << " :)";
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}