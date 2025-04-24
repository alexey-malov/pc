#include <coroutine>
#include <iostream>

namespace
{

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
		~promise_type() { std::cout << "~promise_type\n"; }
		ReturnObject get_return_object()
		{
			std::cout << "get_return_object\n";
			return { CoroHandle::from_promise(*this) };
		}
		std::suspend_never initial_suspend()
		{
			std::cout << "initial_suspend\n";
			return {};
		}
		std::suspend_always final_suspend() noexcept
		{
			std::cout << "final_suspend\n";
			return {};
		}

		void return_void()
		{
			std::cout << "return_void\n";
		}

		void unhandled_exception()
		{
			std::cout << "unhandled_exception\n";
		}
	};

	ReturnObject(CoroHandle h)
		: m_handle(h)
	{
		std::cout << "ReturnObject " << this << ", handle: " << m_handle.address() << "\n";
	}

	ReturnObject(const ReturnObject&) = delete;
	ReturnObject& operator=(const ReturnObject&) = delete;

	ReturnObject(ReturnObject&& other) noexcept
		: m_handle(std::exchange(other.m_handle, nullptr))
	{
		std::cout << "ReturnObject move " << this << "\n";
	}
	ReturnObject& operator=(ReturnObject&& other)
	{
		std::cout << "ReturnObject move assignment " << this << "\n";
		if (this != &other)
		{
			if (m_handle)
			{
				m_handle.destroy();
			}
			m_handle = std::exchange(other.m_handle, nullptr);
		}
		return *this;
	}

	ReturnObject(ReturnObject&&) = delete;

	~ReturnObject()
	{
		if (m_handle)
		{
			m_handle.destroy();
		}
	}

private:
	CoroHandle m_handle;
};

ReturnObject Coroutine()
{
	std::cout << "Coroutine\n";
	co_return;
}

ReturnObject Coroutine(int arg1, int arg2)
{
}

} // namespace

int main()
{
	auto result = Coroutine();
}
