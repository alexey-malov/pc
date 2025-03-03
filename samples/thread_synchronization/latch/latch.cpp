#include <chrono>
#include <iostream>
#include <latch>
#include <mutex>
#include <queue>
#include <syncstream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

int main()
{
	std::mutex m;
	std::queue<int> gifts;
	for (int i = 1; i <= 20; ++i)
		gifts.push(i);

	std::latch latch(gifts.size());
	std::jthread santaClaus{ [&latch] {
		latch.wait(); // Ждем завершения обработки подарков
		std::osyncstream(std::cout) << "Santa delivers the gifts\n";
	} };

	// Эльфы собирают подарки
	std::vector<std::jthread> elves;
	for (int elf = 1; elf <= 3; ++elf)
		elves.emplace_back([&gifts, &m, &latch, elf] {
			for (std::unique_lock lk{ m }; !gifts.empty(); lk.lock())
			{
				int gift = gifts.front();
				gifts.pop();
				lk.unlock();

				std::this_thread::sleep_for(120ms + 200ms * elf);
				std::osyncstream(std::cout) << "Elf " << elf << " have packed the gift " << gift << "\n";
				latch.count_down();
			}
			std::this_thread::sleep_for(1s);
			std::osyncstream(std::cout) << "Elf " << elf << " goes home\n";
		});
}