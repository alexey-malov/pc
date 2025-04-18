#include <future>
#include <iostream>

int Add(int x, int y)
{
	return x + y;
}

int main()
{
	std::packaged_task<int(int, int)> add(Add);
	auto result = add.get_future();
	// Здесь будет deadlock, так как add не запущена.
	std::cout << result.get() << "\n";
}
