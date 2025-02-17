#include <cassert>
#include <iostream>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stack>
#include <stdexcept>
#include <syncstream>
#include <thread>

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
	TSStack& operator=(const TSStack&) = delete;

	void Push(T value)
	{
		std::lock_guard lk{ m_mutex };
		m_stack.push(std::move(value));
	}

	bool IsEmpty() const
	{
		std::shared_lock lk{ m_mutex };
		return m_stack.empty();
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

private:
	std::stack<T> m_stack;
	mutable std::shared_mutex m_mutex;
};

int main()
{
	TSStack<int> stack;
	std::jthread producer{ [&stack] {
		for (int i = 0; i < 11'000'000; ++i)
		{
			stack.Push(i);
		}
	} };

	std::jthread consumer1{ [&stack] {
		for (int i = 0; i < 10'000'000; ++i)
		{
			int value;
			if (stack.TryPop(value))
			{
				// use value
			}
		}
	} };
	std::jthread consumer2{ [&stack] {
		for (int i = 0; i < 10'000'000; ++i)
		{
			if (int value; stack.TryGetTop(value))
			{
				// use value
			}
			if (auto value = stack.TryGetTop())
			{
				// use value
			}
		}
	} };
	for (int i = 0; i < 10'000'000; ++i)
	{
		if (std::unique_ptr<int> value = stack.TryPop())
		{
			// use value
		}
	}
}
