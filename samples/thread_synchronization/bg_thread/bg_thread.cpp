#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <stop_token>
#include <syncstream>
#include <thread>
#include <tuple>

using namespace std::chrono_literals;

void Divide(int x, int y, std::promise<int> promise)
{
	std::cout << "Divide thread id:" << std::this_thread::get_id() << "\n";
	if (y == 0)
		promise.set_exception(std::make_exception_ptr(std::invalid_argument("Division by zero")));
	else
		promise.set_value(x / y);
}

class FunctionWrapper
{
public:
	template <typename F>
	FunctionWrapper(F&& f)
		: m_impl(std::make_unique<Impl<F>>(std::move(f)))
	{
	}

	FunctionWrapper() = default;
	FunctionWrapper(FunctionWrapper&&) = default;
	FunctionWrapper& operator=(FunctionWrapper&&) = default;
	FunctionWrapper(FunctionWrapper&) = delete;
	FunctionWrapper(const FunctionWrapper&) = delete;
	FunctionWrapper& operator=(const FunctionWrapper&) = default;

	void operator()() const { m_impl->Call(); }

private:
	struct ImplBase
	{
		virtual ~ImplBase() = default;
		virtual void Call() = 0;
	};
	template <typename F>
	struct Impl : ImplBase
	{
		F func;
		Impl(F&& f)
			: func(std::move(f))
		{
		}
		void Call() override { func(); }
	};

	std::unique_ptr<ImplBase> m_impl;
};

class ThreadExecutor
{
public:
	ThreadExecutor()
		: m_thread{ std::bind_front(&ThreadExecutor::ThreadProc, this) }
	{
	}

	~ThreadExecutor()
	{
		m_thread.request_stop();
		m_tasksUpdated.notify_one();
	}

	template <typename Fn>
	auto ExecuteAsync(Fn fn)
	{
		std::packaged_task<std::invoke_result_t<Fn>()> task(std::move(fn));
		auto result = task.get_future();
		{
			std::lock_guard lk{ m_mutex };
			m_tasks.emplace(std::move(task));
		}
		m_tasksUpdated.notify_one();
		return result;
	}

private:
	void ThreadProc(std::stop_token stopToken)
	{
		for (std::unique_lock lk{ m_mutex };; lk.lock())
		{
			m_tasksUpdated.wait(lk, [this, &stopToken] {
				return stopToken.stop_requested() || !m_tasks.empty();
			});
			if (m_tasks.empty())
				break;
			auto task = std::move(m_tasks.front());
			m_tasks.pop();
			lk.unlock();
			task();
		}
	}

	std::mutex m_mutex;
	// Или std::queue<std::move_only_function<void()>>
	std::queue<FunctionWrapper> m_tasks;
	std::condition_variable m_tasksUpdated;
	std::jthread m_thread;
};

int main()
{
	std::cout << "Main thread id:" << std::this_thread::get_id() << "\n";
	ThreadExecutor executor;

	auto f1 = executor.ExecuteAsync([] {
		return 10 + 20;
	});

	auto f2 = executor.ExecuteAsync([]() -> int {
		throw std::logic_error("Not implemented");
	});
	auto f3 = executor.ExecuteAsync([] {
		return 42;
	});
	try
	{
		std::cout << f1.get() << "\n"
				  << f2.get() << "\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
	std::cout << f3.get() << "\n";
}
