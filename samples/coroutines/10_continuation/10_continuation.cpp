#include <cassert>
#include <coroutine>
#include <iostream>

struct Task
{
	struct promise_type
	{
		std::coroutine_handle<> continuation; // Хендл корутины для продолжения

		Task get_return_object()
		{
			return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
		}
		std::suspend_always initial_suspend() { return {}; }

		auto final_suspend() noexcept
		{
			struct Awaiter
			{
				bool await_ready() noexcept { return false; }
				void await_suspend(std::coroutine_handle<promise_type> coro) const noexcept
				{
					if (auto next = coro.promise().continuation)
						next.resume();
				}
				void await_resume() noexcept {}
			};
			return Awaiter{};
		}
		void return_void() {}
		void unhandled_exception()
		{
			std::terminate(); // Для примера
		}
		~promise_type()
		{
			std::cout << "promise destroyed\n";
		}
	};

	std::coroutine_handle<promise_type> handle;

	Task(std::coroutine_handle<promise_type> h)
		: handle(h)
	{
	}
	~Task()
	{
		if (handle)
			handle.destroy();
	}

	void SetContinuation(std::coroutine_handle<> h)
	{
		handle.promise().continuation = h;
	}

	void Resume()
	{
		handle.resume();
	}
};

Task SecondCoroutine()
{
	std::cout << "Second coroutine started\n";
	co_return;
}

Task FirstCoroutine()
{
	std::cout << "First coroutine started\n";
	co_await std::suspend_always{}; // приостанавливаемся
	std::cout << "First coroutine resumed\n";
	co_return;
}

int main()
{
	Task first = FirstCoroutine();
	Task second = SecondCoroutine();

	// Устанавливаем continuation: после завершения first возобновляется second
	first.SetContinuation(second.handle);
	std::cout << "resume first\n";
	first.Resume(); // Запускаем первую корутину
	first.Resume();
	assert(first.handle.done());
	assert(second.handle.done());
	std::cout << "exit from main\n";
}
