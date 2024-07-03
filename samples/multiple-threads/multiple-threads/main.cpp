#include <thread>
#include <syncstream>
#include <iostream>

int main()
{
	std::osyncstream syncOut{std::cout};
	std::jthread worker1{[&syncOut] {
		syncOut << "Hello" << std::endl;
	}};
	
	std::jthread worker2{[&syncOut] {
		syncOut << " world" << std::endl;
	}};

	return 0;
}
