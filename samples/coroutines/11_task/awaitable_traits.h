#pragma once
#include <type_traits>
#include <coroutine>

namespace detail
{
// Helper type that can be cast-to from any type.
struct any
{
	template <typename T>
	any(T&&) noexcept
	{
	}
};

template <typename T>
struct is_coroutine_handle
	: std::false_type
{
};

template <typename PROMISE>
struct is_coroutine_handle<std::coroutine_handle<PROMISE>>
	: std::true_type
{
};

// NOTE: We're accepting a return value of coroutine_handle<P> here
// which is an extension supported by Clang which is not yet part of
// the C++ coroutines TS.
template <typename T>
struct is_valid_await_suspend_return_value
	: std::disjunction<std::is_void<T>, std::is_same<T, bool>, is_coroutine_handle<T>>
{
};

template <typename T, typename = std::void_t<>>
struct is_awaiter : std::false_type
{
};

// NOTE: We're testing whether await_suspend() will be callable using an
// arbitrary coroutine_handle here by checking if it supports being passed
// a coroutine_handle<void>. This may result in a false-result for some
// types which are only awaitable within a certain context.
template <typename T>
struct is_awaiter<T,
	std::void_t<decltype(std::declval<T>().await_ready()),
		decltype(std::declval<T>().await_suspend(std::declval<std::coroutine_handle<>>())),
		decltype(std::declval<T>().await_resume())>>
	: std::conjunction<std::is_constructible<bool,
						   decltype(std::declval<T>().await_ready())>,
		  detail::is_valid_await_suspend_return_value<decltype(std::declval<T>().await_suspend(
			  std::declval<std::coroutine_handle<>>()))>>
{
};

template <typename T>
auto get_awaiter_impl(T&& value, int) noexcept(noexcept(static_cast<T&&>(value).operator co_await()))
	-> decltype(static_cast<T&&>(value).operator co_await())
{
	return static_cast<T&&>(value).operator co_await();
}

template <typename T>
auto get_awaiter_impl(T&& value, long) noexcept(noexcept(operator co_await(static_cast<T&&>(value))))
	-> decltype(operator co_await(static_cast<T&&>(value)))
{
	return operator co_await(static_cast<T&&>(value));
}

template <
	typename T,
	std::enable_if_t<is_awaiter<T&&>::value, int> = 0>
T&& get_awaiter_impl(T&& value, any) noexcept
{
	return static_cast<T&&>(value);
}

template <typename T>
auto get_awaiter(T&& value) noexcept(noexcept(detail::get_awaiter_impl(static_cast<T&&>(value), 123)))
	-> decltype(detail::get_awaiter_impl(static_cast<T&&>(value), 123))
{
	return detail::get_awaiter_impl(static_cast<T&&>(value), 123);
}
} // namespace detail

template <typename T, typename = void>
struct awaitable_traits
{
};

template <typename T>
struct awaitable_traits<T, std::void_t<decltype(detail::get_awaiter(std::declval<T>()))>>
{
	using awaiter_t = decltype(detail::get_awaiter(std::declval<T>()));

	using await_result_t = decltype(std::declval<awaiter_t>().await_resume());
};