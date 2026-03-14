#include <chrono>
#include <future>
#include <iostream>
#include <syncstream>
#include <thread>

int Compute()
{
	std::osyncstream(std::cout) << "Start computations in thread "
								<< std::this_thread::get_id() << "\n";

	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::osyncstream(std::cout) << "Finish computations\n";

	return 42;
}

int main()
{
	// Запускаем вычисление асинхронно
	auto future = std::async(std::launch::async, Compute);

	// Выполняем какие-то другие задачи в главном потоке
	std::osyncstream(std::cout) << "Main thread " << std::this_thread::get_id()
								<< " is doing other work...\n";

	// Получаем результат вычисления
	int result = future.get();

	// Используем результат
	std::osyncstream(std::cout) << "Use result: " << result << "\n";
}
