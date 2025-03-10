#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <stop_token>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;

void Divide(int x, int y, std::promise<int> promise)
{
	std::cout << "Divide thread id:" << std::this_thread::get_id() << "\n";
	if (y == 0)
		promise.set_exception(std::make_exception_ptr(std::invalid_argument("Division by zero")));
	else
		promise.set_value(x / y);
}

int main()
{
	std::cout << "Main thread id:" << std::this_thread::get_id() << "\n";

	try
	{
		std::promise<int> promise1;
		auto result1 = promise1.get_future();
		std::jthread worker1{ Divide, 30, 4, std::move(promise1) };
		std::cout << result1.get() << "\n";
		std::promise<int> promise2;
		auto result2 = promise2.get_future();
		std::jthread worker2{ Divide, 30, 0, std::move(promise2) };
		std::cout << result2.get() << "\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}
