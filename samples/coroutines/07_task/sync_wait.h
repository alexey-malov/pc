#pragma once
#include "awaitable_traits.h"
#include "manual_reset_event.h"
#include "task.h"
#include <cassert>
#include <coroutine>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace detail
{

template <typename Result>
class SyncWaitTask;

template <typename Result>
class SyncWaitTaskPromise
{
	using CoroHandle = std::coroutine_handle<SyncWaitTaskPromise<Result>>;

public:
	using reference = Result&&;

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
				coroutine.promise().m_event->Set();
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

	reference GetResult()
	{
		if (m_exception)
		{
			std::rethrow_exception(m_exception);
		}
		else if (m_result)
		{
			return static_cast<reference>(*m_result);
		}
		else
		{
			throw std::logic_error("No value");
		}
	}

private:
	ManualResetEvent* m_event = nullptr;
	std::remove_reference_t<Result>* m_result = nullptr;
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

template <
	typename AWAITABLE,
	typename RESULT = typename awaitable_traits<AWAITABLE&&>::await_result_t,
	std::enable_if_t<!std::is_void_v<RESULT>, int> = 0>
SyncWaitTask<RESULT> MakeSyncWaitTask(AWAITABLE&& awaitable)
{
	co_yield co_await std::forward<AWAITABLE>(awaitable);
}

template <
	typename AWAITABLE,
	typename RESULT = typename awaitable_traits<AWAITABLE&&>::await_result_t,
	std::enable_if_t<std::is_void_v<RESULT>, int> = 0>
SyncWaitTask<void> MakeSyncWaitTask(AWAITABLE&& awaitable)
{
	co_await std::forward<AWAITABLE>(awaitable);
}

} // namespace detail

template <typename AWAITABLE>
auto SyncWait(AWAITABLE&& awaitable)
	-> typename awaitable_traits<AWAITABLE&&>::await_result_t
{
	auto task = detail::MakeSyncWaitTask(std::forward<AWAITABLE>(awaitable));
	ManualResetEvent event;
	task.Start(event);
	event.Wait();
	return task.GetResult();
}