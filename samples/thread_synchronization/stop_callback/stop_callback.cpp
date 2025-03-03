#include <chrono>
#include <iostream>
#include <latch>
#include <mutex>
#include <queue>
#include <stop_token>
#include <string>
#include <syncstream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

int main()
{
	std::jthread worker{
		[](std::stop_token stopToken) {
			std::stop_callback cb{ stopToken,
				[] { std::osyncstream(std::cout) << "Stop requested\n"; } };
			while (!stopToken.stop_requested())
				std::this_thread::sleep_for(1s);
			std::osyncstream(std::cout) << "Worker has finished working\n";
		}
	};
	std::this_thread::sleep_for(500ms);
}
