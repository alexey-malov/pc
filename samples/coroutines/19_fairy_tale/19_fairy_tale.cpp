#include <coroutine>
#include <iostream>
#include <queue>
#include <string>

class Scheduler
{
public:
	void Schedule(std::coroutine_handle<> h)
	{
		m_tasks.push(h);
	}

	void Run()
	{
		while (!m_tasks.empty())
		{
			auto h = m_tasks.front();
			m_tasks.pop();
			if (h && !h.done())
			{
				h.resume();
			}
		}
	}

private:
	std::queue<std::coroutine_handle<>> m_tasks;
};

struct Awaiter
{
	Scheduler& scheduler;

	bool await_ready() const noexcept { return false; }

	void await_suspend(std::coroutine_handle<> h) const noexcept
	{
		scheduler.Schedule(h);
	}

	void await_resume() const noexcept {}
};

Awaiter suspend_and_reschedule(Scheduler& scheduler)
{
	return Awaiter{ scheduler };
}

struct Task
{
	struct promise_type;
	using handle_type = std::coroutine_handle<promise_type>;

	handle_type m_coroutine;
	Scheduler* m_scheduler;

	Task(handle_type h, Scheduler& scheduler)
		: m_coroutine(h)
		, m_scheduler(&scheduler)
	{
	}
	Task(Task&& other) noexcept
		: m_coroutine(other.m_coroutine)
		, m_scheduler(other.m_scheduler)
	{
		other.m_coroutine = nullptr;
		other.m_scheduler = nullptr;
	}
	Task& operator=(Task&& other) noexcept
	{
		if (this != &other)
		{
			if (m_coroutine)
				m_coroutine.destroy();
			m_coroutine = other.m_coroutine;
			m_scheduler = other.m_scheduler;
			other.m_coroutine = nullptr;
			other.m_scheduler = nullptr;
		}
		return *this;
	}
	~Task()
	{
		if (m_coroutine)
			m_coroutine.destroy();
	}
	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;

	void Start()
	{
		if (m_scheduler)
		{
			m_scheduler->Schedule(m_coroutine);
		}
	}

	struct promise_type
	{
		Scheduler* m_scheduler = nullptr;

		auto get_return_object()
		{
			return Task{ handle_type::from_promise(*this), *m_scheduler };
		}

		std::suspend_never initial_suspend() noexcept { return {}; } // запуск сразу
		std::suspend_always final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() { std::terminate(); }

		void set_scheduler(Scheduler& scheduler)
		{
			m_scheduler = &scheduler;
		}
	};
};

Task RedRidingHoodStory(Scheduler& scheduler)
{
	static const char* phrases[] = {
		"Red Riding Hood leaves the house.",
		"Red Riding Hood arrived at grandma's.",
		"The wolf noticed Riding Hood and decided to run ahead of her.",
		"The wolf ran to grandma's first and ate her!",
		"The hunter came and saved grandma and Riding Hood.",
		"The fairy tale is over."
	};

	for (const char* phrase : phrases)
	{
		std::cout << phrase << std::endl;
		co_await suspend_and_reschedule(scheduler);
	}
}

int main()
{
	Scheduler scheduler;

	auto task = RedRidingHoodStory(scheduler);
	task.Start();

	scheduler.Run();
}
