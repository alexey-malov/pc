#include <future>
#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

void PrintThreadId(int taskIndex)
{
	std::osyncstream(std::cout)
		<< "Task " << taskIndex
		<< ", thread id: " << std::this_thread::get_id() << "\n";
}

int main()
{
	constexpr int taskCount = 20;

	std::osyncstream(std::cout)
		<< "Main thread id: " << std::this_thread::get_id() << "\n";

	std::vector<std::future<void>> futures;
	futures.reserve(taskCount);

	for (int i = 1; i <= taskCount; ++i)
	{
		futures.push_back(std::async(std::launch::async, PrintThreadId, i));
	}

	for (auto& future : futures)
	{
		future.get(); // Ждём завершения каждой фоновой задачи
	}
}