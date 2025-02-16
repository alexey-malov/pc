#include <iostream>
#include <thread>

int main()
{
	std::jthread t1{ [] {
		std::cout << "A";
	} };
	// Завершение потока, связанного с t1, синхронизируется с возвратом из join
	t1.join(); // Вызов t1.join() упорядочен перед выводом B
	std::cout << "B";
}
