// sync-stream.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

void OutputUnsynchronized()
{
	std::vector<std::jthread> threads;
	for (int threadIndex = 1; threadIndex <= 9; ++threadIndex)
	{
		threads.emplace_back([threadIndex] {
			for (size_t i = 0; i < 100; ++i)
			{
				std::cout << "Thread " << threadIndex << ": " << i << "\n";
			}
		});
	}
}

void OutputSynchronized()
{
	std::vector<std::jthread> threads;
	for (int threadIndex = 1; threadIndex <= 9; ++threadIndex)
	{
		threads.emplace_back([threadIndex] {
			for (size_t i = 0; i < 100; ++i)
			{
				std::osyncstream(std::cout) << "Thread " << threadIndex << ": " << i << "\n";
			}
		});
	}
}

int main()
{
	OutputUnsynchronized();
	std::cout << "---\n";
	OutputSynchronized();
}
