#include <iostream>
#include <memory>
#include <string>
#include <thread>

class BgTask
{
public:
	explicit BgTask(std::string name)
		: m_name(std::move(name))
	{
	}

	void operator()()
	{
		std::cout << "Hello from bg task " << m_name << std::endl;
	}

private:
	std::string m_name;
};

void TestCopyableTask()
{
	BgTask bgTask{ "Task 1" };
	// Объект bgTask будет скопирован внутрь потока
	std::jthread t{ bgTask };
}

class MoveOnlyTask
{
public:
	explicit MoveOnlyTask(std::unique_ptr<int> data)
		: m_data(std::move(data))
	{
	}

	void operator()()
	{
		std::cout << "Hello from MoveOnlyTask with data: " << *m_data << std::endl;
	}

private:
	std::unique_ptr<int> m_data;
};

void TestMoveOnlyTask()
{
	MoveOnlyTask task{ std::make_unique<int>(42) };
	// Объект bgTask будет перемещён внутрь потока
	std::jthread t{ std::move(task) };
}

void TestLambda()
{
	std::jthread t{ [] {
		std::cout << "Hello from lambda\n";
	} };
}

int main()
{
	TestCopyableTask();
	TestMoveOnlyTask();
}