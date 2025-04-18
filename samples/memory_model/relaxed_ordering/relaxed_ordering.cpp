#include <assert.h>
#include <atomic>
#include <iomanip>
#include <iostream>
#include <thread>

std::atomic<bool> x = false, y = false;
std::atomic<int> z = 0;

int main()
{
	std::jthread a([] {
		x.store(true, std::memory_order_relaxed);
		y.store(true, std::memory_order_relaxed);
	});
	std::jthread b([] {
		while (!y.load(std::memory_order_relaxed))
			;
		if (x.load(std::memory_order_relaxed))
			++z;
	});
	a.join();
	b.join();
	std::cout << std::boolalpha << x << ' ' << y << ' ' << z.load() << "\n";
}