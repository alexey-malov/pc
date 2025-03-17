#include <iostream>
#include <memory>
#include <thread>

int x = 0, y = 0;
int a = 1, b = 2;

int main()
{
	std::jthread t1{ [] {
		a = 3;
		b = 4;
	} };
	std::jthread t2{ [] {
		x = b;
		y = a;
	} };
	t1.join();
	t2.join();
	std::cout << "x: " << x << ", y: " << y << "\n";
}
