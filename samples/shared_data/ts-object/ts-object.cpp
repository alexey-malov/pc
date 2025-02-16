#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <syncstream>
#include <thread>

using Clock = std::chrono::high_resolution_clock;
using Seconds = std::chrono::duration<double>;
using namespace std::chrono;

class TSObject
{
public:
	int GetIntData() const
	{
		std::lock_guard lk{ m_mutex };
		return m_intData;
	}

	void SetIntData(int data)
	{
		std::lock_guard lk{ m_mutex };
		m_intData = data;
	}

	std::string GetStringData() const
	{
		std::lock_guard lk{ m_mutex };
		return m_stringData;
	}

	void SetStringData(std::string data)
	{
		std::lock_guard lk{ m_mutex };
		m_stringData = std::move(data);
	}

private:
	int m_intData = 0;
	std::string m_stringData;
	mutable std::mutex m_mutex;
};

int main()
{
	TSObject obj;

	std::jthread t{ [&obj] {
		for (int i = 0; i < 1'000'000; ++i)
		{
			obj.SetIntData(i * 2);
			obj.SetStringData(std::to_string(i) + "s");
		}
	} };
	for (int i = 0; i < 1'000'000; ++i)
	{
		obj.SetIntData(i);
		obj.SetStringData(std::to_string(i));
	}
	t.join();
	// Выводимый результат может различаться от запуска к запуску
	std::cout << obj.GetIntData() << ": " << obj.GetStringData() << "\n";
}
