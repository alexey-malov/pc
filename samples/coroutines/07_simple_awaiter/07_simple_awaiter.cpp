#include <coroutine>
#include <iostream>

// Awaiter объект
struct SimpleAwaiter
{
	bool await_ready() const noexcept
	{
		std::cout << "await_ready (false)\n";
		return false; // возвращаем false — значит, выполнение будет приостановлено
	}

	// Приостанавливает выполнение текущей корутины
	void await_suspend(std::coroutine_handle<> handle) const noexcept
	{
		std::cout << "await_suspend\n";
		// Здесь могли бы, например, сохранить handle и возобновить позже
		handle.resume(); // для примера — сразу возобновляем
		std::cout << "exit from await_suspend\n";
	}

	int await_resume() const noexcept
	{
		std::cout << "await_resume: 42\n";
		return 42; // возвращаем значение из co_await
	}
};

struct ReturnObject
{
	struct promise_type
	{
		ReturnObject get_return_object() { return {}; }
		std::suspend_never initial_suspend()
		{
			std::cout << "initial_suspend\n";
			return {};
		}
		std::suspend_never final_suspend() noexcept
		{
			std::cout << "final_suspend\n";
			return {};
		}
		void return_void() {}
		void unhandled_exception() {}
	};
};

ReturnObject DoSomething()
{
	int result = co_await SimpleAwaiter{};
	std::cout << "co_await returned: " << result << '\n';
}

int main()
{
	DoSomething();
}
