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

int Divide(int x, int y)
{
	std::cout << "Divide thread id:" << std::this_thread::get_id() << "\n";
	if (y == 0)
		throw std::invalid_argument("Division by zero");
	return x / y;
}

int main()
{
	std::cout << "Main thread id:" << std::this_thread::get_id() << "\n";

	try
	{
		std::packaged_task div(Divide);
		auto result = div.get_future();
		std::jthread worker{ std::move(div), 20, 0 };
		std::cout << result.get() << "\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}
