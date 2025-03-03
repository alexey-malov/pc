#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <syncstream>
#include <thread>
#include <queue>

using namespace std::chrono_literals;

int main()
{
	using Task = std::packaged_task<void()>;

	std::mutex m;
	std::queue<Task> tasks;
	std::condition_variable cv;

	std::jthread worker{ [&cv, &m, &tasks](std::stop_token stoken) {
		std::unique_lock lk{ m };
		for (;;)
		{
			cv.wait(lk, [&tasks, &stoken] {
				return stoken.stop_requested() || tasks.empty();
			});
			if (stoken.stop_requested())
				return;
			auto task = std::move(tasks.front());
			tasks.pop();
			lk.unlock();
			task();
		}
	} };
}