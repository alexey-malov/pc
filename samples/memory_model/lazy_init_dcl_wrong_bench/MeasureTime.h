#pragma once
#include <chrono>
#include <iostream>
#include <string>

class Timer
{
	using Clock = std::chrono::high_resolution_clock;

public:
	explicit Timer(std::string name)
		: m_name(std::move(name))
	{
	}

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	~Timer()
	{
		Stop();
	}

	void Stop()
	{
		if (m_running)
		{
			m_running = false;
			auto cur = Clock::now();
			auto dur = cur - m_start;
			std::cout << m_name << " took " << dur << " (" << std::chrono::duration<double>(dur)
					  << ")" << std::endl;
		}
	}

private:
	std::string m_name;
	Clock::time_point m_start = Clock::now();
	bool m_running = true;
};

template <typename Fn, typename... Args>
decltype(auto) MeasureTime(std::string name, Fn&& fn, Args&&... args)
{
	Timer t{ std::move(name) };
	return std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
}
