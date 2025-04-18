#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stack>
#include <stdexcept>
#include <string>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;

template <typename T>
class TSStack
{
public:
	TSStack() = default;
	TSStack(const TSStack& other)
	{
		std::shared_lock lk{ other.m_mutex };
		m_stack = other.m_stack;
	}

	TSStack& operator=(const TSStack& rhs)
	{
		if (this != std::addressof(rhs))
		{
			std::scoped_lock lk{ m_mutex, rhs.m_mutex };
			m_stack = rhs.m_stack;
		}
		return *this;
	}

	void Push(T value)
	{
		{
			std::lock_guard lk{ m_mutex };
			m_stack.push(std::move(value));
		}
		m_dataCond.notify_one(); // Уведомляем один из потоков о появлении данных
	}

	bool IsEmpty() const
	{
		std::shared_lock lk{ m_mutex };
		return m_stack.empty();
	}

	size_t GetSize() const
	{
		std::shared_lock lk{ m_mutex };
		return m_stack.size();
	}

	[[nodiscard]] bool TryGetTop(T& out) const
	{
		std::shared_lock lk{ m_mutex };
		if (m_stack.empty())
			return false;
		out = m_stack.top();
		return true;
	}

	[[nodiscard]] std::optional<T> TryGetTop() const
	{
		std::shared_lock lk{ m_mutex };
		if (m_stack.empty())
			return std::nullopt;
		return m_stack.top();
	}

	bool TryPop(T& out)
	{
		std::lock_guard lk{ m_mutex };
		if (m_stack.empty())
			return false;
		if constexpr (std::is_nothrow_move_assignable_v<T>)
			out = std::move(m_stack.top());
		else
			out = m_stack.top();
		m_stack.pop();
		return true;
	}

	std::unique_ptr<T> TryPop()
	{
		std::lock_guard lk{ m_mutex };
		if (m_stack.empty())
			return nullptr;
		std::unique_ptr<T> value;
		if constexpr (std::is_nothrow_move_constructible_v<T>)
			value = std::make_unique<T>(std::move(m_stack.top()));
		else
			value = std::make_unique<T>(m_stack.top());
		m_stack.pop();
		return value;
	}

	void WaitAndPop(T& out)
	{
		std::unique_lock lk{ m_mutex };
		m_dataCond.wait(lk, [this] { return !m_stack.empty(); });
		out = m_stack.top();
		m_stack.pop();
	}

	std::unique_ptr<T> WaitAndPop()
	{
		std::unique_lock lk{ m_mutex };
		m_dataCond.wait(lk, [this] { return !m_stack.empty(); });
		std::unique_ptr<T> value;
		if constexpr (std::is_nothrow_move_constructible_v<T>)
			value = std::make_unique<T>(std::move(m_stack.top()));
		else
			value = std::make_unique<T>(m_stack.top());
		m_stack.pop();
		lk.unlock();
		return value;
	}

private:
	std::stack<T> m_stack;
	mutable std::shared_mutex m_mutex;
	// Для использования с shared_mutex требуется condition_variable_any
	std::condition_variable_any m_dataCond;
};

int main()
{
	TSStack<int> stack;

	std::jthread producer{ [&stack] {
		for (int i = 0; i < 10; ++i)
		{
			stack.Push(i);
			std::this_thread::sleep_for(200ms);
		}
	} };

	std::jthread consumer{ [&stack] {
		for (int i = 0; i < 10; ++i)
		{
			int value;
			stack.WaitAndPop(value);
			std::cout << value << "\n";
			std::this_thread::sleep_for(500ms);
		}
	} };
}
