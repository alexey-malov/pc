#include <chrono>
#include <iostream>
#include <latch>
#include <mutex>
#include <queue>
#include <stop_token>
#include <syncstream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

int main()
{
	std::jthread worker{ [](std::stop_token stopToken) {
		for (int i = 1; !stopToken.stop_requested(); ++i)
		{
			std::osyncstream(std::cout) << "Worker works: " << i << "\n";
			std::this_thread::sleep_for(500ms);
		}
		std::cout << "Worker has finished working\n";
	} };

	std::this_thread::sleep_for(3s);

	worker.request_stop(); // Если не вызвать явно, то это сделает деструктор
}