#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;

int main()
{
	std::mutex m;
	bool flag = false;
	std::condition_variable cv;

	std::jthread waitingThread{ [&m, &flag, &cv] {
		std::unique_lock lk{ m };
		cv.wait(lk); // Никогда так не делайте, wait может завершиться до получения уведомления
		assert(flag == true); // Условие (flag == true) может не сработать
	} };

	std::this_thread::sleep_for(1s);

	{
		std::lock_guard lk{ m };
		flag = true;
	}

	cv.notify_one(); // Уведомляем ожидающий поток
}
