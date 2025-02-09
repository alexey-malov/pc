#include <iostream>
#include <memory>
#include <string>
#include <thread>

class Func
{
	int& m_dataRef;

public:
	Func(int& data)
		: m_dataRef(data)
	{
	}
	void operator()()
	{
		for (int i = 0; i < 100000; ++i)
		{
			std::cout << m_dataRef << std::endl;
		}
	}
};

int main()
{
	int data = 42;
	std::thread(Func{data}).detach();
	// При выходе из main Func::m_dataRef будет ссылаться на разрушенную переменную data
}