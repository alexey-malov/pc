#include <boost/asio/steady_timer.hpp>
#include <boost/cobalt/main.hpp>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;
namespace asio = boost::asio;
namespace cobalt = boost::cobalt;

// co_main - точка входа в асинхронную программу.
// Она будет вызвана из функции main, предоставляемой cobalt
cobalt::main co_main(int argc, char* argv[])
{
	std::cout << "co_main started\n";
	asio::steady_timer timer(co_await cobalt::this_coro::executor, 2s);
	std::cout << "Starting timer\n";
	co_await timer.async_wait(cobalt::use_op);
	std::cout << "Timer completed\n";
	std::cout << "co_main finished\n";
	co_return 0;
}
