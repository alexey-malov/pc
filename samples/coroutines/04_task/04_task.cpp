#include <coroutine>
#include <iostream>


template <typename T>
class Task
{
public:
	struct promise_type
	{
		T result; // Тут хранится результат
		Task get_return_object() { return Task{ std::coroutine_handle<promise_type>::from_promise(*this) }; }
		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void return_value(T value) { result = std::move(value); }
		void unhandled_exception() { std::terminate(); }
	};

	T GetResult() { return m_handle.promise().result; }

	void Resume()
	{
		if (m_handle && !m_handle.done())
		{
			m_handle.resume();
		}
	}

	Task() = default;

	explicit Task(std::coroutine_handle<promise_type> handle)
		: m_handle(handle)
	{
	}

	Task(Task&& other) noexcept
		: m_handle(std::exchange(other.m_handle, nullptr))
	{
	}
	Task& operator=(Task&& other) noexcept
	{
		if (this != std::addressof(other))
		{
			m_handle = std::exchange(other.m_handle, nullptr);
		}
		return *this;
	}

	~Task()
	{
		if (m_handle)
			m_handle.destroy();
	}

private:
	std::coroutine_handle<promise_type> m_handle = nullptr;
};

Task<int> Add(int a, int b)
{
	std::cout << "Add " << a << " + " << b << "\n";
	co_return a + b;
}
#if 0

Task<int> Calculate()
{
	auto x = Add(2, 3);
	auto y = Add(5, 10);
	return co_await x + co_await y;
}
#endif

int main()
{

}
