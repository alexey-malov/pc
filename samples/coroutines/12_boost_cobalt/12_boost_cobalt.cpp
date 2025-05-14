#include <boost/cobalt/run.hpp>
#include <boost/cobalt/task.hpp>
#include <chrono>
#include <iostream>
#include <thread>

using boost::cobalt::task;
using namespace std::chrono_literals;

task<int> SumAsync(int x, int y)
{
	std::this_thread::sleep_for(1s);
	co_return x + y;
}

task<void> SayHello()
{
	std::cout << "Hello " << (co_await SumAsync(30, 12)) << "\n";
	co_return;
}

int main()
{
	boost::cobalt::run(SayHello());
}
