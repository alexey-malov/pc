#include <coroutine>
#include <iostream>

template <typename T>
class Generator
{
public:
	struct promise_type;
	using CoroHandle = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		T currentValue;

		template <typename U>
		std::suspend_always yield_value(U&& value)
		{
			currentValue = std::forward<U>(value);
			return {};
		}
		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void unhandled_exception() {}
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

	bool HasNext() const
	{
		return m_handle && !m_handle.done();
	}

	T GetNext()
	{
		if (HasNext())
		{
			m_handle.resume();
			return m_handle.promise().currentValue;
		}
		throw std::out_of_range("No more values");
	}

private:
	CoroHandle m_handle = nullptr;
};

namespace
{

Generator<int> GenIntegers(int from, int step, int count)
{
	for (int i = 0, cur = from; i < count; ++i, cur += step)
	{
		co_yield cur;
	}
}

Generator<int> GetFib()
{
	int a = 0, b = 1;
	for (;;)
	{
		co_yield a;
		a = std::exchange(b, a + b);
	}
}

} // namespace

int main()
{
	for (auto gen = GenIntegers(1, 1, 10); gen.HasNext();)
	{
		std::cout << gen.GetNext() << " ";
	}

	std::cout << "\n";

	auto fib = GetFib();
	for (int i = 0; i < 10; ++i)
	{
		std::cout << fib.GetNext() << " ";
	}
}
