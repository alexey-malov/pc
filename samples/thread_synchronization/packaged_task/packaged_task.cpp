#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <stop_token>
#include <syncstream>
#include <thread>

int Add(int x, int y)
{
	std::cout << "Add thread id:" << std::this_thread::get_id() << "\n";
	return x + y;
}

int main()
{
	std::cout << "Main thread id:" << std::this_thread::get_id() << "\n";
	std::packaged_task<int(int, int)> add(Add);
	auto result = add.get_future();
	std::jthread worker{ std::move(add), 10, 20 };
	worker.join();
	std::cout << result.get() << "\n";
}
