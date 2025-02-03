#include <iostream>
#include <thread>

void SayHello()
{
	std::cout << "Hello, world\n";
}

int main()
{
	std::jthread t{SayHello};
	// Деструктор объекта t автоматически вызовет метод join()
}
