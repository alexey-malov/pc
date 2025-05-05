#pragma once
#include "task.h"
#include <cassert>
#include <condition_variable>
#include <coroutine>
#include <mutex>
#include <stdexcept>
#include <type_traits>
#include <utility>

class ManualResetEvent
{
public:
	void Set()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_isSet = true;
		m_cond.notify_all();
	}

	void Wait()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait(lock, [this] { return m_isSet; });
	}

	void Reset()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_isSet = false;
	}

private:
	std::mutex m_mutex;
	std::condition_variable m_cond;
	bool m_isSet = false;
};

namespace detail
{

template <typename Result>
class SyncWaitTask;

template <typename Result>
class SyncWaitTaskPromise
{
	using CoroHandle = std::coroutine_handle<SyncWaitTaskPromise<Result>>;

public:
	void Start(ManualResetEvent& event)
	{
		m_event = &event;
		CoroHandle::from_promise(*this).resume();
	}

	SyncWaitTask<Result> get_return_object() noexcept
	{
		return SyncWaitTask<Result>{ CoroHandle::from_promise(*this) };
	}

	std::suspend_always initial_suspend() { return {}; }

	auto final_suspend() noexcept
	{
		struct CompletionNotifier
		{
			bool await_ready() const noexcept { return false; }
			void await_suspend(CoroHandle coroutine) noexcept
			{
				coroutine.promise().m_event.Set();
			}
			void await_resume() noexcept {}
		};
		return CompletionNotifier{};
	}

	void return_void()
	{
		// The coroutine should have either yielded a value or thrown
		// an exception in which case it should have bypassed return_void().
		assert(false);
	}

	auto yield_value(Result&& value)
	{
		m_result = std::addressof(value);
		return final_suspend();
	}

	void unhandled_exception()
	{
		m_exception = std::current_exception();
	}

	Result&& GetResult()
	{
		if (m_exception)
		{
			std::rethrow_exception(m_exception);
		}
		else if (m_result)
		{
			return std::move(*m_result);
		}
		else
		{
			throw std::logic_error("No value");
		}
	}

private:
	ManualResetEvent* m_event = nullptr;
	Result* m_result = nullptr;
	std::exception_ptr m_exception;
};

template <>
class SyncWaitTaskPromise<void>
{
	using CoroHandle = std::coroutine_handle<SyncWaitTaskPromise<void>>;

public:
	void Start(ManualResetEvent& event)
	{
		m_event = &event;
		CoroHandle::from_promise(*this).resume();
	}

	SyncWaitTask<void> get_return_object() noexcept;

	std::suspend_always initial_suspend() { return {}; }

	auto final_suspend() noexcept
	{
		struct CompletionNotifier
		{
			bool await_ready() const noexcept { return false; }
			void await_suspend(CoroHandle coroutine) noexcept
			{
				coroutine.promise().m_event->Set();
			}
			void await_resume() noexcept {}
		};
		return CompletionNotifier{};
	}

	void return_void()
	{
	}

	void unhandled_exception()
	{
		m_exception = std::current_exception();
	}

	void GetResult()
	{
		if (m_exception)
		{
			std::rethrow_exception(m_exception);
		}
	}

private:
	ManualResetEvent* m_event = nullptr;
	std::exception_ptr m_exception;
};

template <typename Result>
class SyncWaitTask
{
public:
	using promise_type = SyncWaitTaskPromise<Result>;
	using CoroHandle = std::coroutine_handle<promise_type>;

	explicit SyncWaitTask(CoroHandle handle)
		: m_handle(handle)
	{
		assert(m_handle);
	}
	SyncWaitTask(const SyncWaitTask&) = delete;
	SyncWaitTask& operator=(const SyncWaitTask&) = delete;
	SyncWaitTask(SyncWaitTask&& other) noexcept
		: m_handle(std::exchange(other.m_handle, nullptr))
	{
	}
	SyncWaitTask& operator=(SyncWaitTask&& other) noexcept
	{
		if (this != std::addressof(other))
		{
			if (m_handle)
			{
				m_handle.destroy();
			}
			m_handle = std::exchange(other.m_handle, nullptr);
		}
		return *this;
	}
	~SyncWaitTask()
	{
		if (m_handle)
			m_handle.destroy();
	}

	void Start(ManualResetEvent& event)
	{
		m_handle.promise().Start(event);
	}

	decltype(auto) GetResult()
	{
		assert(m_handle);
		return m_handle.promise().GetResult();
	}

private:
	CoroHandle m_handle;
};

inline SyncWaitTask<void> SyncWaitTaskPromise<void>::get_return_object() noexcept
{
	return SyncWaitTask<void>{ CoroHandle::from_promise(*this) };
}

template <typename T,
	std::enable_if_t<!std::is_void_v<T>, int> = 0>
SyncWaitTask<T> MakeSyncWaitTask(Task<T>&& task)
{
	co_yield co_await std::move(task);
}

template <typename T,
	std::enable_if_t<std::is_void_v<T>, int> = 0>
SyncWaitTask<T> MakeSyncWaitTask(Task<T>&& task)
{
	co_await std::move(task);
}

} // namespace detail

template <typename T>
auto SyncWait(Task<T>&& task)
{
	auto syncWaitTask = detail::MakeSyncWaitTask(std::move(task));

	ManualResetEvent event;
	syncWaitTask.Start(event);
	event.Wait();
	return syncWaitTask.GetResult();
}
