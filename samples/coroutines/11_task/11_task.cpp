#include "task.h"
#include "sync_wait.h"
#include <iostream>

Task<int> Add(int x, int y)
{
	std::cout << "Run Inner x: " << x << ", y: " << y << "\n";
	co_return x + y;
}

Task<int> Outer()
{
	std::cout << "Enter outer\n";
	auto r1 = Add(10, 20);
	auto r2 = Add(6, 6);
	std::cout << "co_awaiting\n";
	auto result = (co_await r1) + (co_await r2);
	std::cout << "Exit outer\n";
	co_return result;
}

Task<void> VoidFn()
{
	co_return;
}

int main()
{
	try
	{
		auto outer = Outer();
		auto result = SyncWait(outer);
		std::cout << result << "\n";
		SyncWait(VoidFn());
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << '\n';
	}
}
