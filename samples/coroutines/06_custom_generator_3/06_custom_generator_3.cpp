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
			return !std::holds_alternative<std::monostate>(result);
		}

		void ThrowIfException() const
		{
			if (HasException())
			{
				std::rethrow_exception(std::get<std::exception_ptr>(result));
			}
		}

		T& GetValue()
		{
			ThrowIfException();
			if (HasValue())
			{
				return std::get<T>(result);
			}
			throw std::logic_error("GetValue() is called without resume");
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

	class Iterator
	{
	public:
		// Даём генератору доступ к нашему приватному конструктору
		friend class Generator;

		using iterator_category = std::input_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		Iterator() = default;

		// Поддерживается только перемещение итератора
		Iterator(const Iterator&) = delete;
		Iterator& operator=(const Iterator&) = delete;

		Iterator(Iterator&& other) noexcept
			: m_handle(std::exchange(other.m_handle, nullptr))
		{
		}
		Iterator& operator=(Iterator&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_handle = std::exchange(other.m_handle, nullptr);
			}
			return *this;
		}
		~Iterator() = default;

		reference operator*() const
		{
			assert(m_handle);
			return m_handle.promise().GetValue();
		}

		pointer operator->() const
		{
			return &operator*();
		}

		Iterator& operator++()
		{
			assert(m_handle);
			assert(!m_handle.done());

			m_handle.resume();
			if (m_handle.done())
			{
				// Если корутина завершилась, то нужно проверить, не выбросила ли она исключение
				auto handle = std::exchange(m_handle, nullptr);
				handle.promise().ThrowIfException();
			}

			return *this;
		}

		bool operator==(std::default_sentinel_t) const noexcept
		{
			return m_handle == nullptr;
		}

	private:
		explicit Iterator(CoroHandle handle)
			: m_handle(handle)
		{
			operator++();
		}

		CoroHandle m_handle = nullptr;
	};

	// Возобновляет корутину и возвращает итератор на значение.
	// Вызвать begin() можно только один раз.
	Iterator begin()
	{
		return Iterator{ m_handle };
	}

	std::default_sentinel_t end()
	{
		return std::default_sentinel;
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
		for (auto value : GenIntegers())
		{
			std::cout << value << " ";
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << '\n';
	}
	std::cout << "Done\n";
}
