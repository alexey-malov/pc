#include <coroutine>
#include <iostream>

class ReturnObject
{
public:
	struct promise_type;
	using CoroHandle = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		promise_type()
		{
			std::cout << "promise_type " << this << "\n";
		}

		promise_type(const promise_type&) = delete;
		promise_type& operator=(const promise_type&) = delete;

		~promise_type()
		{
			std::cout << "~promise_type\n";
		}

		// Этот метод возвращает объект, который будет возвращён из корутины.
		ReturnObject get_return_object()
		{
			std::cout << "get_return_object\n";
			// Получаем указатель на coroutne handle и возвращаем ReturnObject вместе с ним
			return { CoroHandle::from_promise(*this) };
		}
		// Этот метод вызывается, когда корутина приостанавливается при входе в неё.
		std::suspend_never initial_suspend()
		{
			std::cout << "initial_suspend\n";
			return {};
		}
		// Этот метод вызывается, когда корутина приостанавливается при выходе из неё.
		std::suspend_always final_suspend() noexcept
		{
			std::cout << "final_suspend\n";
			return {};
		}

		// Этот метод вызывается, когда корутина завершает выполнение.
		void return_void()
		{
			std::cout << "return_void\n";
		}
		// Этот метод вызывается, когда в корутине возникает исключение.
		void unhandled_exception()
		{
			std::cout << "unhandled_exception\n";
		}
	};

	ReturnObject(CoroHandle h)
		: m_handle(h)
	{
		std::cout << "ReturnObject " << this << " - "
				  << m_handle.address() << "\n ";
	}

	ReturnObject(ReturnObject&& other) noexcept
		: m_handle(std::exchange(other.m_handle, nullptr))
	{
		std::cout << "ReturnObject move " << this << "\n";
	}

	~ReturnObject()
	{
		std::cout << "~ReturnObject " << this << "\n";
		if (m_handle)
		{
			m_handle.destroy();
		}
	}

	bool Resume()
	{
		if (!m_handle)
		{
			return false;
		}
		if (!m_handle.done())
		{
			m_handle.resume();
		}
		return !m_handle.done();
	}

private:
	CoroHandle m_handle;
};

void NestedFunc()
{
	int i = 0;
	std::cout << "Enter NestedFunc, addrof(i)=" << &i << "\n";
}

ReturnObject ResumableFunc()
{
	int i = 0;
	std::cout << "Enter ResumableFunc, addrof(i)=" << &i << "\n";
	NestedFunc();
	co_await std::suspend_always{};
	NestedFunc();
	std::cout << "Exit ResumableFunc" << &i << "\n";
}

int main()
{
	int i = 0;
	std::cout << "addrof (i) " << &i << "\n";

	auto result = ResumableFunc();

	std::cout << "Returned from ResumableFunc\n";
	while (result.Resume())
	{
		std::cout << "ResumableFunc resumed\n";
	}
}
