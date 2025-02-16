#include <cassert>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <stack>
#include <stdexcept>
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
		if (m_stack.empty())
			throw std::logic_error("Stack is empty");
		m_stack.pop();
	}

	T GetTop() const
	{
		std::shared_lock lk{ m_mutex };
		if (m_stack.empty())
			throw std::logic_error("Stack is empty");
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
		for (int i = 0; i < 11'000'000; ++i)
		{
			stack.Push(i);
		}
	} };

	int exceptionCount = 0;
	std::mutex m;

	auto consume = [&stack, &m, &exceptionCount] {
		for (int i = 0; i < 10'000'000; ++i)
		{
			try
			{
				if (!stack.IsEmpty())
				{
					std::ignore = stack.GetTop();
					stack.Pop();
				}
			}
			catch (const std::exception&)
			{
				std::lock_guard lk{ m };
				++exceptionCount;
			}
		}
	};

	std::jthread consumer1{ consume };
	consume();
	consumer1.join();
	std::cout << exceptionCount << "\n";
}
