#include <cassert>
#include <iostream>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stack>
#include <stdexcept>
#include <string>
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


	TSStack& operator=(const TSStack& rhs)
	{
		if (this != std::addressof(rhs))
		{
			std::lock_guard thisLock{ m_mutex };
			std::shared_lock otherLock(rhs.m_mutex);
			m_stack = rhs.m_stack;
		}
		return *this;
	}

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

private:
	std::stack<T> m_stack;
	mutable std::shared_mutex m_mutex;
};

int main()
{
	TSStack<int> stack1;
	TSStack<int> stack2;

	for (int i = 0; i < 10; ++i)
	{
		stack1.Push(i);
		stack2.Push(-i);
	}
	static constexpr int repeatCount = 10'000;
	std::jthread t{ [&stack1, &stack2] {
		for (int i = 0; i < repeatCount; ++i)
		{
			stack2.Push(i);
			stack1 = stack2;
			std::ignore = stack2.TryPop();
		}
	} };

	for (int i = 0; i < repeatCount; ++i)
	{
		stack1.Push(-i);
		stack2 = stack1;
		std::ignore = stack1.TryPop();
	}
	t.join();
	std::cout << "Done\n";
}
