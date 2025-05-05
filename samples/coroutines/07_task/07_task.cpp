#include "task.h"
#include "sync_wait.h"
#include <iostream>

Task<int> Inner()
{
	std::cout << "Enter inner\n";
	co_return 42;
}

Task<void> Outer()
{
	std::cout << "Enter outer\n";
	int result = co_await Inner();
	std::cout << result << "\n";
	std::cout << "Exit outer\n";
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
