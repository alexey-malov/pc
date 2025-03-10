#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <stop_token>
#include <syncstream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

int main()
{
	std::promise<std::string> stringPromise;
	std::vector<std::string> strings;
	try
	{
		stringPromise.set_value(strings.at(0));
	}
	catch (...)
	{
		stringPromise.set_exception(std::current_exception());
	}

	try
	{
		std::cout << stringPromise.get_future().get() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
