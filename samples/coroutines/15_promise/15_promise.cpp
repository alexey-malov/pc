#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/cobalt/main.hpp>
#include <boost/cobalt/promise.hpp>
#include <boost/cobalt/wait_group.hpp>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;
namespace asio = boost::asio;
namespace cobalt = boost::cobalt;
using SteadyClock = std::chrono::steady_clock;
using Seconds = std::chrono::duration<double>;

cobalt::promise<void> RoastCutlet()
{
	std::cout << "Roasting cutlet for 3 seconds\n";

	asio::steady_timer timer(co_await cobalt::this_coro::executor, 3s);
	co_await timer.async_wait(cobalt::use_op);

	std::cout << "Cutlet is ready\n";
}

cobalt::promise<void> BakeBread()
{
	std::cout << "Baking bread for 2 seconds\n";

	asio::steady_timer timer(co_await cobalt::this_coro::executor, 2s);
	co_await timer.async_wait(cobalt::use_op);

	std::cout << "Bread is ready\n";
}

cobalt::promise<void> AssemblyHamburger()
{
	std::cout << "Assemblyng hamburger for 1 second\n";

	asio::steady_timer timer(co_await cobalt::this_coro::executor, 1s);
	co_await timer.async_wait(cobalt::use_op);

	std::cout << "Hamburger is assebled\n";
}

cobalt::main co_main(int /*argc*/, char* /*argv*/[])
{
	auto start = SteadyClock::now();
	std::cout << "Cooking hamburger...\n";

	cobalt::wait_group wg; // Групповое ожидание promise<void>
	wg.push_back(RoastCutlet());
	wg.push_back(BakeBread());

	std::cout << "Waiting for cutlet and bread\n";
	co_await wg.wait();

	co_await AssemblyHamburger();

	auto end = SteadyClock::now();
	std::cout << "Hamburger was cooked in "
			  << Seconds(end - start) << " seconds\n";
	co_return 0;
}
