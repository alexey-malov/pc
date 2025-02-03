#include <iostream>
#include <thread>

void SayHello()
{
	std::cout << "Hello, world\n";
}

int main()
{
	std::thread t{ SayHello };
	t.detach(); // Отцепляем поток от объекта std::thread
}
