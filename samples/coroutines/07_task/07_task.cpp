#include "task.h"
#include "sync_wait.h"
#include <iostream>

Task<int> Add(int x, int y)
{
	std::cout << "Enter inner\n";
	co_return x + y;
}

Task<void> Outer()
{
	std::cout << "Enter outer\n";
	auto r1 = Add(10, 20);
	auto r2 = Add(6, 6);
	auto result = (co_await r1) + (co_await r2);
	std::cout << result << "\n";
	std::cout << "Exit outer\n";
	co_return;
}

int main()
{
	try
	{
		auto result = Outer();
		SyncWait(std::move(result));
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << '\n';
	}
}
