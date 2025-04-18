#include "stack.h"
#include <iostream>

int main()
{
	Stack<int> stack;
	stack.Push(42);
	auto value = stack.Pop();
	std::cout << *value << std::endl;
}
