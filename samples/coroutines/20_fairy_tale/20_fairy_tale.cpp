#include <coroutine>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <string>

struct Task
{
	struct promise_type
	{
		Task get_return_object() { return Task{ std::coroutine_handle<promise_type>::from_promise(*this) }; }
		std::suspend_always initial_suspend() noexcept { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() { std::exit(1); }
	};

	std::coroutine_handle<promise_type> coro;

	explicit Task(std::coroutine_handle<promise_type> h)
		: coro(h)
	{
	}
	Task(Task&& t) noexcept
		: coro(t.coro)
	{
		t.coro = nullptr;
	}
	~Task()
	{
		if (coro)
			coro.destroy();
	}

	bool Resume()
	{
		if (!coro || coro.done())
			return false;
		coro.resume();
		return !coro.done();
	}
};

// Простой Scheduler, который хранит задачи в очередь и запускает по очереди
class Scheduler
{
	std::queue<Task> m_tasks;

public:
	void Schedule(Task&& task)
	{
		m_tasks.push(std::move(task));
	}

	void Run()
	{
		while (!m_tasks.empty())
		{
			Task task = std::move(m_tasks.front());
			m_tasks.pop();
			if (task.Resume())
			{
				Schedule(std::move(task)); // если не завершена — ставим в конец очереди
			}
		}
	}
};

struct DialogAwaiter
{
	bool await_ready() { return false; }
	void await_suspend(std::coroutine_handle<>) {}
	void await_resume() {}
};

void Print(const std::string& speaker, const std::string& line)
{
	std::cout << speaker << ": \"" << line << "\"\n";
}

// Корутины персонажей
Task RedHood()
{
	Print("Red Hood", "You know, Grandma always said: \"Don't wander in the woods alone.\" Well, I did.");
	co_await DialogAwaiter{};
	Print("Red Hood", "Life's no spa, right?");
	co_await DialogAwaiter{};

	Print("Red Hood", "Hey, fox nose, I'm not your prey.");
	co_await DialogAwaiter{};
	Print("Red Hood", "And Grandma's no fragile lady. We come in peace... kinda.");
	co_await DialogAwaiter{};
}

Task Wolf()
{
	Print("Wolf", "Well, well... Who's this brave little red walking into my turf?");
	co_await DialogAwaiter{};
	Print("Wolf", "In my woods, everything's red... but few walk away.");
	co_await DialogAwaiter{};
}

Task Grandma()
{
	Print("Grandma", "Who dares come here to eat me? This is my house, and I set the rules.");
	co_await DialogAwaiter{};
	Print("Grandma", "You better watch your step, wolfie.");
	co_await DialogAwaiter{};
}

Task Hunter()
{
	Print("Hunter", "Everyone calm down. I'm the one who puts the period in this story.");
	co_await DialogAwaiter{};
}

// Координация диалога
Task Story(Scheduler& scheduler)
{
	scheduler.Schedule(RedHood());
	scheduler.Schedule(Wolf());
	scheduler.Schedule(Grandma());
	scheduler.Schedule(Hunter());

	// Ждём, пока все диалоги сыграны
	co_return;
}

int main()
{
	Scheduler scheduler;

	scheduler.Schedule(Story(scheduler));
	scheduler.Run();
	std::cout << "\n*The End*\n";
	return 0;
}
