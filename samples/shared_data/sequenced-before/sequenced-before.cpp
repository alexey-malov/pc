#include <iostream>

int x = 0;

int f()
{
	x += 10;
	return x;
}

int g()
{
	x += 20;
	return x;
}

int h(int y)
{
	x += y;
}

int main()
{
	int a = 5;
	int b = a + 10; // (a + 10) упорядочено перед модификацией b

	// g() упорядочено перед h().
	// f() + h(g()) + x упорядочено перед модификацией x
	// f(), h() и чтение x не упорядочены друг относительо друга
	x = f() + h(g()) + x;
}
