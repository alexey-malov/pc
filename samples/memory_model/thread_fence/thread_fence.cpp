#include <assert.h>
#include <atomic>
#include <iomanip>
#include <iostream>
#include <thread>

std::atomic_int data = 0;
std::atomic_bool ready = false;

int main()
{
	std::jthread a([] {
		data.store(42, std::memory_order_relaxed);

		// Всё, что выше, выполнится ДО того, что ниже
		std::atomic_thread_fence(std::memory_order_release);

		ready.store(true, std::memory_order_relaxed);
	});
	std::jthread b([] {
		while (!ready.load(std::memory_order_relaxed))
			/* Ждём */;

		// Всё, что ниже, выполнится после после
		std::atomic_thread_fence(std::memory_order_acquire);

		std::cout << data.load(std::memory_order_relaxed);
	});
}