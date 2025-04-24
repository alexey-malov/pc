#include <coroutine>
#include <iostream>

namespace
{
struct Promise;
using CoroHandle = std::coroutine_handle<Promise>;

struct ReturnObject
{
	struct promise_type
	{
		ReturnObject get_return_object() { return {}; }
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { return {}; }
		void unhandled_exception() {}
		void return_void() {}
	};
};

struct Awaiter
{
	std::coroutine_handle<>* handle;
	constexpr bool await_ready() const noexcept { return false; }
	// Этот метод вызывается, когда корутина приостанавливается.
	void await_suspend(std::coroutine_handle<> h) noexcept
	{
		std::cout << "Awaiter::await_suspend: " << h.address() << "\n";
		*handle = h;
	}
	constexpr void await_resume() noexcept {}
};

struct Logger
{
	Logger() { std::cout << "Logger\n"; }
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	~Logger() { std::cout << "~Logger\n"; }
};

/**
 * Эта функция является корутиной, так как использует оператор co_await.
 * Она бесконечно выполняет цикл, приостанавливаясь на каждой итерации.
 */
ReturnObject ResumableFunction(std::coroutine_handle<>* out)
{
	std::cout << "Enter ResumableFunction\n";
	Awaiter awaiter{ out };
	Logger logger;
	for (unsigned i = 0;; ++i)
	{
		std::cout << "ResumableFunction co_await awaiter\n";
		co_await awaiter;
		std::cout << "ResumableFunction: " << i << std::endl;
	}

	std::cout << "Exit ResumableFunction\n";
}

} // namespace

int main()
{
	// Этот хэндл служит для управления корутиной
	std::coroutine_handle<> h;
	ResumableFunction(&h);
	
	std::cout << "In main: got coroutine handle: " << h.address() << "\n";

	for (unsigned i = 0; i < 5; ++i)
	{
		std::cout << "In main()\n";
		// Передаём управление обратно в корутину.
		h();
	}

	// Разрушаем корутину.
	h.destroy();
}
