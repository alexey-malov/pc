#include <cassert>
#include <coroutine>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <variant>

template <typename T>
class Generator
{
public:
	struct promise_type;
	using CoroHandle = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		std::variant<std::monostate, T, std::exception_ptr> result;

		template <typename U>
		std::suspend_always yield_value(U&& value)
		{
			result.emplace<1>(std::forward<U>(value));
			return {};
		}

		bool HasException() const noexcept
		{
			return std::holds_alternative<std::exception_ptr>(result);
		}

		bool HasValue() const noexcept
		{
			return std::holds_alternative<T>(result);
		}

		T& GetValue()
		{
			if (HasException())
			{
				std::rethrow_exception(std::get<std::exception_ptr>(result));
			}
			else if (HasValue())
			{
				return std::get<T>(result);
			}
			throw std::logic_error("No value");
		}

		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void unhandled_exception()
		{
			result = std::current_exception();
		}
		Generator get_return_object()
		{
			return Generator{ CoroHandle::from_promise(*this) };
		}
		void return_void() {}
	};

	explicit Generator(CoroHandle handle)
		: m_handle(handle)
	{
	}

	// Генератор нельзя копировать. Только перемещать.
	Generator(const Generator& other) = delete;
	Generator& operator=(const Generator& other) = delete;

	Generator(Generator&& other) noexcept
		: m_handle(std::exchange(other.m_handle, nullptr))
	{
	}
	Generator& operator=(Generator&& other) noexcept
	{
		if (this != std::addressof(other))
		{
			m_handle = std::exchange(other.m_handle, nullptr);
		}
		return *this;
	}

	~Generator()
	{
		if (m_handle)
		{
			m_handle.destroy();
		}
	}

	bool HasValue() const noexcept
	{
		assert(m_handle);
		m_handle.resume();

		return !m_handle.done() || m_handle.promise().HasException();
	}

	T& GetValue()
	{
		assert(m_handle);
		return m_handle.promise().GetValue();
	}

private:
	CoroHandle m_handle = nullptr;
};

namespace
{

Generator<int> GenIntegers()
{
	co_yield 1;
	co_yield 2;
	throw std::runtime_error("Out of numbers");
}

} // namespace

int main()
{
	try
	{
		for (auto gen = GenIntegers(); gen.HasValue();)
		{
			std::cout << gen.GetValue() << " ";
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << '\n';
	}
	std::cout << "Done\n";
}
