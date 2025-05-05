#if 1

#include <coroutine>
#include <iostream>

struct Task
{
	struct promise_type
	{
		std::coroutine_handle<> continuation;

		Task get_return_object()
		{
			return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
		}
		std::suspend_always initial_suspend() { return {}; }

		auto final_suspend() noexcept
		{
			struct Awaiter
			{
				std::coroutine_handle<> continuation;
				bool await_ready() noexcept { return false; }
				void await_suspend(std::coroutine_handle<>) const noexcept
				{
					if (continuation)
						continuation.resume();
				}
				void await_resume() noexcept {}
			};
			return Awaiter{ continuation };
		}
		void return_void() {}
		void unhandled_exception()
		{
			std::terminate();
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
	first.Resume(); // Запускаем первую корутину
	first.Resume();
}


#else
#include <coroutine>
#include <iostream>
#include <utility>

struct ReturnObject
{
	struct promise_type
	{
		#if 0
		struct FinalAwaiter
		{
			bool await_ready() const noexcept
			{
				return false; // возвращаем false
			}
			std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> coro) noexcept
			{
				return coro.promise().continuation;
			}
			void await_resume() noexcept {}
		};
		#endif

		ReturnObject get_return_object()
		{
			std::cout << "get_return_object\n";
			return { std::coroutine_handle<promise_type>::from_promise(*this) };
		}
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() {}

		std::coroutine_handle<> continuation;
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

	#if 0
	auto operator co_await() const noexcept
	{
		struct Awaitable
		{
			bool await_ready() const noexcept
			{
				return !coroutine || !coroutine.done();
			}
			std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> coro) noexcept
			{
				coro.promise().continuation = coro;
				return coro.promise().continuation;
			}
			void await_resume() noexcept {}

			std::coroutine_handle<promise_type> coroutine;
		};
		return Awaitable{};
	}
	#endif

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
	void await_suspend(std::coroutine_handle<ReturnObject::promise_type> coro) const noexcept
	{
		coro.promise().continuation = coro; // сохраняем корутину
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
#endif