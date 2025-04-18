#include "LockFreeStack.h"
#include <iostream>

struct S
{
	size_t refCounter;
	char* ptr;
};

int main()
{
	std::atomic<std::shared_ptr<int>> p;
	std::atomic<S> ps;

	std::cout << p.is_lock_free() << "\n";
	std::cout << ps.is_lock_free() << "\n";
	LockFreeStack<int> stack;
	stack.Push(42);
	auto value = stack.Pop();
	std::cout << *value << std::endl;
}
