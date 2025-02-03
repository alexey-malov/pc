#include <iostream>
#include <thread>

void SayHello()
{
	std::cout << "Hello, world\n";
}

int main()
{
	std::thread t{SayHello};
	t.join(); // Ждём завершения работы потока
}
