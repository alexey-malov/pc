#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

class SpinMutex
{
public:
	void lock() noexcept
	{
		while (m_flag.test_and_set())
		{
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
			m_flag.wait(true);
#endif
		}
	}

	bool try_lock() noexcept
	{
		return !m_flag.test_and_set();
	}

	void unlock() noexcept
	{
		m_flag.clear();
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
		m_flag.notify_one();
#endif
	}

private:
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

int main()
{
	int counter = 0;
	SpinMutex mutex;

	std::thread t1{ [&counter, &mutex] {
		for (int i = 0; i < 100'000; ++i)
		{
			std::lock_guard lk{ mutex };
			++counter;
		}
	} };
	for (int i = 0; i < 100'000; ++i)
	{
		std::lock_guard lk{ mutex };
		--counter;
	}
	t1.join();
	std::cout << counter << "\n"; // Выведет 0
}
