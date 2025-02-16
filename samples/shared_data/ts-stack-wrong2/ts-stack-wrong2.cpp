#include <cassert>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <stack>
#include <syncstream>
#include <thread>

template <typename T>
class TSStack
{
public:
	void Push(T value)
	{
		std::unique_lock lk{ m_mutex };
		m_stack.push(std::move(value));
	}

	bool IsEmpty() const
	{
		std::shared_lock lk{ m_mutex };
		return m_stack.empty();
	}

	void Pop()
	{
		std::unique_lock lk{ m_mutex };
		assert(!m_stack.empty());
		m_stack.pop();
	}

	T GetTop() const
	{
		std::shared_lock lk{ m_mutex };
		assert(!m_stack.empty());
		return m_stack.top();
	}

private:
	std::stack<T> m_stack;
	mutable std::shared_mutex m_mutex;
};

int main()
{
	TSStack<int> stack;
	std::jthread producer{ [&stack] {
		for (int i = 0; i < 1'000; ++i)
		{
			stack.Push(i);
		}
	} };

	auto consume = [&stack] {
		for (int i = 0; i < 1'000; ++i)
		{
			if (!stack.IsEmpty())
			{
				std::osyncstream(std::cout)
					<< stack.GetTop() << "\n";
				stack.Pop();
			}
		}
	};

	std::jthread consumer1{ consume };
	consume();
}
