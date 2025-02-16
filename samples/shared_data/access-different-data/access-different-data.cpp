#include <iostream>
#include <string>
#include <thread>

int a = 0;
std::string b = "hello";
std::string c = "please";

int main()
{
	std::jthread th1{ [] { a = 1; } };
	std::jthread th2{ [] { b += "!"; } };
	c = "thanks!";
}
