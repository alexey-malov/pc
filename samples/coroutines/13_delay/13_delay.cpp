#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/cobalt/spawn.hpp>
#include <boost/cobalt/task.hpp>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;
namespace asio = boost::asio;
namespace cobalt = boost::cobalt;
using cobalt::task;

task<void> Coroutine()
{
	std::cout << "Coroutine started\n";
	// Получаем executor, связанный с текущей корутиной и создаём на его основе таймер
	asio::steady_timer timer1(co_await cobalt::this_coro::executor, 2s);
	std::cout << "Starting timer\n";
	co_await timer1.async_wait(cobalt::use_op);
	std::cout << "Timer & coroutine complete\n";
}

void DelayWithCoroutine()
{
	asio::io_context ctx;
	// Запускаем задачу Coroutine в контексте ctx и получаем результат в виде std::future
	auto f = cobalt::spawn(ctx, Coroutine(), asio::use_future);
	std::cout << "ctx.run()\n";
	ctx.run();
	std::cout << "f.get()\n";
	f.get();
}

void DelayWithCallback()
{
	std::cout << "DelayWithCallback started\n";

	asio::io_context ctx;
	asio::steady_timer timer(ctx, 2s);
	std::cout << "Starting timer\n";

	// Стандартный способ ожидания в boost::asio - callback
	timer.async_wait([](const boost::system::error_code& ec) {
		if (!ec)
			std::cout << "Timer completed\n";
		else
			std::cout << "Timer cancelled\n";
	});
	std::cout << "context.run()\n";
	ctx.run();
	std::cout << "DelayWithCallback finished\n";
}

void SyncDelay()
{
	asio::io_context ctx;
	asio::steady_timer timer(ctx, 2s);
	std::cout << "Starting sync timer\n";
	timer.wait(); // Блокирующее ожидание
	std::cout << "Timer wait is over\n";
}

int main()
{
	SyncDelay();
	std::cout << "----\n";
	DelayWithCallback();
	std::cout << "----\n";
	DelayWithCoroutine();
}

