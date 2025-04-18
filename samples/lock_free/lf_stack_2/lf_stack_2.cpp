#include "LockFreeStack.h"
#include <iostream>

int main()
{
	LockFreeStack<int> stack;
	stack.Push(42);
	auto value = stack.Pop();
	std::cout << *value << std::endl;
}
