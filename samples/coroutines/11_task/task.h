#pragma once

#include <cassert>
#include <coroutine>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

template <typename T>
class Task;

namespace detail
{

class TaskPromiseBase
{
	struct FinalAwaitable
	{
		bool await_ready() const noexcept { return false; }
		template <typename Promise>
		std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise> coro) noexcept
		{
			return coro.promise().m_continuation;
		}
		void await_resume() noexcept {}
	};

public:
	std::suspend_always initial_suspend() { return {}; }
	FinalAwaitable final_suspend() noexcept { return {}; }
	void SetContinuation(std::coroutine_handle<> continuation) noexcept
	{
		m_continuation = continuation;
	}

private:
	std::coroutine_handle<> m_continuation;
};

template <typename Result>
class TaskPromise final : public TaskPromiseBase
{
public:
	Task<Result> get_return_object();

	void unhandled_exception()
	{
		m_result.template emplace<2>(std::current_exception());
	}

	template <typename U>
	void return_value(U&& value)
	{
		m_result.template emplace<1>(std::forward<U>(value));
	}

	Result& GetResult()&
	{
		if (m_result.index() == 1)
			return std::get<1>(m_result);
		if (m_result.index() == 2)
			std::rethrow_exception(std::get<2>(m_result));
		throw std::logic_error("No value");
	}

	// HACK: Need to have co_await of task<int> return prvalue rather than
	// rvalue-reference to work around an issue with MSVC where returning
	// rvalue reference of a fundamental type from await_resume() will
	// cause the value to be copied to a temporary. This breaks the
	// sync_wait() implementation.

	using RValueType = std::conditional_t<
		std::is_arithmetic_v<Result> || std::is_pointer_v<Result>,
		Result,
		Result&&>;

	RValueType GetResult() &&
	{
		if (m_result.index() == 2)
			std::rethrow_exception(std::get<2>(m_result));
		assert(m_result.index() == 1);
		return std::move(std::get<1>(m_result));
	}

private:
	std::variant<std::monostate, Result, std::exception_ptr> m_result;
};

template <>
class TaskPromise<void> final : public TaskPromiseBase
{
public:
	Task<void> get_return_object();

	void unhandled_exception() noexcept
	{
		assert(!m_exception);
		m_exception = std::current_exception();
	}

	void return_void() noexcept
	{
		assert(!m_exception);
	}

	void GetResult() const
	{
		if (m_exception)
			std::rethrow_exception(m_exception);
	}

private:
	std::exception_ptr m_exception;
};

} // namespace detail

template <typename T>
class Task
{
	friend detail::TaskPromise<T>;

public:
	using promise_type = detail::TaskPromise<T>;

	Task(Task&& coroutine) noexcept
		: m_coroutine{ std::exchange(coroutine, nullptr) }
	{
		assert(m_coroutine);
	}

	Task& operator=(Task&& rhs) noexcept
	{
		if (this != std::addressof(rhs))
		{
			if (m_coroutine)
				m_coroutine.destroy();
			m_coroutine = std::exchange(rhs.m_coroutine, nullptr);
		}
	}

	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;

	~Task()
	{
		if (m_coroutine)
			m_coroutine.destroy();
	}

	auto operator co_await() const& noexcept
	{
		struct Awaitable
		{
			bool await_ready() const noexcept
			{
				return !m_coroutine || m_coroutine.done();
			}
			auto await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept
			{
				m_coroutine.promise().SetContinuation(awaitingCoroutine);
				return m_coroutine;
			}
			decltype(auto) await_resume()
			{
				if (!m_coroutine)
				{
					throw std::logic_error("Coroutine is not valid");
				}
				return m_coroutine.promise().GetResult();
			}
			std::coroutine_handle<promise_type> m_coroutine;
		};

		assert(m_coroutine);

		return Awaitable{ m_coroutine };
	}

private:
	using CoroHandle = std::coroutine_handle<promise_type>;

	explicit Task(CoroHandle coroutine)
		: m_coroutine(coroutine)
	{
	}

	std::coroutine_handle<promise_type> m_coroutine = nullptr;
};

namespace detail
{

template <typename Result>
inline Task<Result> TaskPromise<Result>::get_return_object()
{
	return Task<Result>{ std::coroutine_handle<TaskPromise>::from_promise(*this) };
}

inline Task<void> TaskPromise<void>::get_return_object()
{
	return Task<void>{ std::coroutine_handle<TaskPromise>::from_promise(*this) };
}

} // namespace detail