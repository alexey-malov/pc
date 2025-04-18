#include <atomic>
#include <iostream>
#include <thread>

std::atomic<bool> x = false, y = false;
std::atomic<int> z = 0;

int main()
{
	{
		std::jthread a([] { // Пишем x
			x.store(true, std::memory_order::seq_cst);
		});
		std::jthread b([] { // Пишем y
			y.store(true, std::memory_order::seq_cst);
		});
		std::jthread c{ [] { // Читаем x, а потом y
			while (!x.load(std::memory_order::seq_cst))
				;
			if (y.load(std::memory_order::seq_cst))
				++z;
		} };
		std::jthread d([] { // Читаем y, а потом x
			while (!y.load(std::memory_order::seq_cst))
				;
			if (x.load(std::memory_order::seq_cst))
				++z;
		});
	}
	std::cout << z.load() << "\n";
}
