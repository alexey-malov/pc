#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <stdexcept>
#include <syncstream>
#include <thread>

using namespace std::literals;

#if 1
class HierarchicalMutex
{
public:
	using Hierarchy = unsigned long long;

	explicit HierarchicalMutex(Hierarchy hierarchy)
		: m_hierarchy{ hierarchy }
	{
	}

	void lock()
	{
		CheckForHierarchyViolation();
		m_mutex.lock();
		UpdateHierarchy();
	}

	void unlock()
	{
		if (m_thisThreadHierarchy != m_hierarchy)
			throw std::logic_error("Mutex hierarchy violated");
		m_thisThreadHierarchy = m_prevHierarchy;
		m_mutex.unlock();
	}

	bool try_lock()
	{
		CheckForHierarchyViolation();
		if (!m_mutex.try_lock())
			return false;
		UpdateHierarchy();
		return true;
	}

private:
	void CheckForHierarchyViolation() const
	{
		if (m_thisThreadHierarchy <= m_hierarchy)
			throw std::logic_error("Mutex hierarchy violated");
	}
	void UpdateHierarchy() noexcept
	{
		m_prevHierarchy = m_thisThreadHierarchy;
		m_thisThreadHierarchy = m_hierarchy;
	}

	const Hierarchy m_hierarchy;
	Hierarchy m_prevHierarchy = 0;
	std::mutex m_mutex;
	static inline thread_local Hierarchy m_thisThreadHierarchy = std::numeric_limits<Hierarchy>::max();
};
#else

#endif

HierarchicalMutex highLevelMutex{ 10'000 };
HierarchicalMutex midLevelMutex{ 5'000 };
HierarchicalMutex lowLevelMutex{ 1'000 };
int lowLevelData = 0;

int DoLowLevelStuff()
{
	++lowLevelData;
	return lowLevelData;
}

int LowLevelFunc()
{
	std::lock_guard lk{ lowLevelMutex };
	return DoLowLevelStuff();
}

void DoHighLevelStuff(int data)
{
	std::osyncstream(std::cout) << "Low level data "
								<< data << "\n";
}

void HighLevelFunc()
{
	std::lock_guard lk1{ highLevelMutex };
	DoHighLevelStuff(LowLevelFunc());
}

void DoOtherStuff()
{
}

void OtherFunc()
{
	HighLevelFunc();
	DoOtherStuff();
}

int main()
{
	std::jthread t1{ [] {
		for (int i = 0; i < 100000; ++i)
			HighLevelFunc();
	} };

	std::jthread t2{ [] {
		for (int i = 0; i < 10'000; ++i)
		{
			try
			{
				std::lock_guard lk{ midLevelMutex };
				OtherFunc();
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
	} };
}
