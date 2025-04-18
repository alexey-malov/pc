#include <cassert>
#include <future>
#include <iostream>

int main()
{
	std::future<int> future;
	{
		std::promise<int> promise;
		future = promise.get_future();
	} // Объект promise разрушен до установки исключения или значения
	try
	{
		std::cout << future.get() << "\n";
	}
	catch (const std::future_error& e)
	{
		assert(e.code() == std::future_errc::broken_promise);
		std::cout << e.what() << "\n";
	}
}
