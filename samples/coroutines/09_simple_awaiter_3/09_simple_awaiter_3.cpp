#include <coroutine>
#include <iostream>
#include <utility>

struct ReturnObject
{
	struct promise_type
	{
		ReturnObject get_return_object()
		{
			std::cout << "get_return_object\n";
			return { std::coroutine_handle<promise_type>::from_promise(*this) };
		}
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() {}
	};

	ReturnObject(std::coroutine_handle<promise_type> h) noexcept
		: handle(h)
	{
	}

	ReturnObject(ReturnObject&& other) noexcept
		: handle(std::exchange(other.handle, nullptr))
	{
	}

	ReturnObject(const ReturnObject&) = delete;

	~ReturnObject()
	{
		if (handle)
			handle.destroy();
	}

	std::coroutine_handle<promise_type> handle;
};

struct SimpleAwaitable
{
	bool await_ready() const noexcept
	{
		std::cout << "await_ready (false)\n";
		return false; // возвращаем false
	}

	// Приостанавливает выполнение текущей корутины
	void await_suspend(std::coroutine_handle<>) const noexcept
	{
		std::cout << "await_suspend\n";
	}

	int await_resume() const noexcept
	{
		std::cout << "await_resume: 42\n";
		return 42; // возвращаем значение из co_await
	}
};

ReturnObject DoSomething()
{
	int result = co_await SimpleAwaitable{};
	std::cout << "co_await returned: " << result << '\n';
}

int main()
{
	auto ret = DoSomething();
	std::cout << "resuming\n";
	ret.handle.resume();
	std::cout << "exit from main\n";
}
