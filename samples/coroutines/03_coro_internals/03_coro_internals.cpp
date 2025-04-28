#include <cassert>
#include <coroutine>
#include <iostream>
#include <utility>

template <typename T>
class Task
{
public:
	struct promise_type
	{
		T result; // Тут хранится результат
		Task get_return_object() { return Task{ std::coroutine_handle<promise_type>::from_promise(*this) }; }
		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void return_value(T value) { result = std::move(value); }
		void unhandled_exception() { std::terminate(); }
	};

	T GetResult() { return m_handle.promise().result; }

	void Resume()
	{
		if (m_handle && !m_handle.done())
		{
			m_handle.resume();
		}
	}

	Task() = default;

	explicit Task(std::coroutine_handle<promise_type> handle)
		: m_handle(handle)
	{
	}

	Task(Task&& other) noexcept
		: m_handle(std::exchange(other.m_handle, nullptr))
	{
	}
	Task& operator=(Task&& other) noexcept
	{
		if (this != std::addressof(other))
		{
			m_handle = std::exchange(other.m_handle, nullptr);
		}
		return *this;
	}

	~Task()
	{
		if (m_handle)
			m_handle.destroy();
	}

private:
	std::coroutine_handle<promise_type> m_handle = nullptr;
};

Task<int> Coroutine(int arg1, int arg2)
{
	int res = arg1 + arg2;
	co_return res;
}

int main()
{
	auto task = Coroutine(40, 2);
	task.Resume();
	std::cout << task.GetResult() << "\n";
}

struct CoroutineState
{
	struct Frame
	{
		int arg1;
		int arg2;
		int res;
	};

	Frame frame;

	Task<int>::promise_type promise;
};

/**
 * Это очень условный код, иллюстрирующий внутреннее устройство корутины.
 * Компилятор разобъет корутину на несколько частей,
 * используя места вызов co_await/co_return/co_yield
 * и будет переключаться между ними при возобновлении корутины.
 * Состояние корутины будет сохранено в Coroutine state
 */
Task<int> GeneratedCoroutine(int arg1, int arg2)
{
	// 1. Выделение сырой памяти под состояние корутины
	CoroutineState* state = static_cast<CoroutineState*>(operator new(sizeof(CoroutineState)));

	// 2. Копируем аргументы в фрейм
	// Строго говоря, компилятор вызывает конструктор копирования или перемещения, но здесь упрощено
	state->frame.arg1 = arg1;
	state->frame.arg2 = arg2;

	try
	{
		// 3. Инициализация promise
		::new (std::addressof(state->promise)) Task<int>::promise_type();
	}
	catch (...)
	{
		// Здесь компилятор ещё дополнительно вызовет деструкторы для всех аргументов

		operator delete(state);
		throw;
	}

	try
	{
		// 4. Вызов initial_suspend
		auto initial = state->promise.initial_suspend();
		if (!initial.await_ready())
		{
			// Компилятор дополнительно запомнит состояние корутины, чтобы потом возобновить её
			return state->promise.get_return_object();
		}

		// 5. Выполнение тела корутины
		// Строго говоря, компилятор вызовет конструктор для инициализации state->frame.res
		state->frame.res = state->frame.arg1 + state->frame.arg2;

		// 6. co_return
		state->promise.return_value(state->frame.res);
	}
	catch (...)
	{
		state->promise.unhandled_exception();
	}

	// 7. Вызов final_suspend
	auto final = state->promise.final_suspend();
	if (!final.await_ready())
	{
		// Приостанавливаем выполнение и возвращаем управление
		return state->promise.get_return_object();
	}

	// 8. Уничтожение promise
	state->promise.~promise_type();

	// 8. Освобождение памяти
	operator delete(state);

	return {};
}
