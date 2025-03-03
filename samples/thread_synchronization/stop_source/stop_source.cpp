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

void Worker(std::stop_token stopToken, std::string name)
{
	while (!stopToken.stop_requested())
	{
		std::osyncstream(std::cout) << name << " works" << "\n";
		std::this_thread::sleep_for(300ms);
	}
	std::osyncstream(std::cout) << name << " finished working\n";
}

int main()
{
	std::stop_source stopSource;

	std::jthread group{
		[](std::stop_token stopToken) {
			std::jthread ivan{ Worker, stopToken, "Ivan" };
			std::jthread peter{ Worker, stopToken, "Peter" };
		},
		stopSource.get_token()
	};

	std::jthread supervisor{ Worker, stopSource.get_token(), "Supervisor" };

	std::this_thread::sleep_for(1s);
	std::osyncstream(std::cout) << "STOP!!!\n";
	stopSource.request_stop();
}