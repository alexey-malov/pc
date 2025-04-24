#include <coroutine>
#include <iostream>

namespace
{

template <typename ResultType>
class Task
{
public:
	struct promise_type
	{
		Task get_return_object() { return {}; }
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void unhandled_exception() {}
		void return_value([[maybe_unused]] ResultType value)
		{
		}
	};
	bool await_ready() const noexcept
	{
		return false;
	}

	ResultType await_resume() noexcept
	{
	}

	void await_suspend(std::coroutine_handle<> h) noexcept
	{
		std::cout << "Task::await_suspend: " << h.address() << "\n";
		h.resume();
	}
	
};

Task<int> GetAnswer()
{
	co_return 42;
}

Task<int> GetAnswer2()
{
	co_return 43;
}

Task<int> GetFinalAnswer()
{
	int value = co_await GetAnswer();
	int value2 = co_await GetAnswer2();
	co_return value + value2;
}

} // namespace

int main()
{
}
