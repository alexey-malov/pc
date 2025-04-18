#include <atomic>
#include <iostream>
#include <thread>

// Этот класс здесь просто для иллюстрации. Настоящий atomic устроен сложнее
template <typename T>
class atomic
{
public:
	void store(T desired, std::memory_order order = std::memory_order_seq_cst) noexcept;

	T load(std::memory_order order = std::memory_order_seq_cst) const noexcept;

	T exchange(T desired, std::memory_order order = std::memory_order_seq_cst) noexcept;

	bool compare_exchange_weak(T& expected, T desired, std::memory_order success, std::memory_order failure) noexcept;
	bool compare_exchange_weak(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) noexcept;

	bool compare_exchange_strong(T& expected, T desired, std::memory_order success, std::memory_order failure) noexcept;
	bool compare_exchange_strong(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) noexcept;

	void wait(T old, std::memory_order order = std::memory_order_seq_cst) const noexcept;
};

int main()
{
}
