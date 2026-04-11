#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadSafeQueue
{
public:
	using Task = std::function<void()>;

	void Push(Task task)
	{
		{
			std::lock_guard lock(m_mutex);
			m_queue.push(std::move(task));
		}
		m_cv.notify_one();
	}

	bool TryPop(Task& task)
	{
		std::lock_guard lock(m_mutex);
		if (m_queue.empty())
			return false;

		task = std::move(m_queue.front());
		m_queue.pop();
		return true;
	}

private:
	std::mutex m_mutex;
	std::queue<Task> m_queue;
	std::condition_variable m_cv;
};

void CpuTask()
{
	constexpr int iterations = 100;

	volatile double x = 1.0;

	for (int i = 0; i < iterations; ++i)
	{
		x = x * 1.0000001 + 0.0000001;
		x = x / 1.0000002 + 0.0000002;
	}
}

// Бенчмарк
void RunBenchmark(int producerCount, int consumerCount, int tasksPerProducer)
{
	ThreadSafeQueue queue;
	std::atomic<bool> done = false;
	std::atomic<int> tasksDone = 0;

	const int totalTasks = producerCount * tasksPerProducer;

	auto startTime = std::chrono::steady_clock::now();

	// Consumers
	std::vector<std::jthread> consumers;
	for (int i = 0; i < consumerCount; ++i)
	{
		consumers.emplace_back([&](std::stop_token) {
			while (!done.load(std::memory_order_relaxed))
			{
				ThreadSafeQueue::Task task;

				if (queue.TryPop(task))
				{
					task();
					tasksDone.fetch_add(1, std::memory_order_relaxed);
				}
				else
				{
					// лёгкий backoff
					std::this_thread::yield();
				}
			}
		});
	}

	// Producers
	std::vector<std::jthread> producers;
	for (int i = 0; i < producerCount; ++i)
	{
		producers.emplace_back([&]() {
			for (int j = 0; j < tasksPerProducer; ++j)
			{
				queue.Push([] {
					CpuTask();
				});
			}
		});
	}

	producers.clear(); // дождались завершения

	// ждём выполнения всех задач
	while (tasksDone.load(std::memory_order_relaxed) < totalTasks)
	{
		std::this_thread::yield();
	}

	done = true;

	auto endTime = std::chrono::steady_clock::now();

	double seconds = std::chrono::duration<double>(endTime - startTime).count();

	std::cout << "Producers: " << producerCount
			  << ", Consumers: " << consumerCount
			  << ", Tasks: " << totalTasks << "\n";

	std::cout << "Time: " << seconds << " sec\n";
	std::cout << "Throughput: "
			  << (totalTasks / seconds)
			  << " tasks/sec\n\n";
}

int main()
{
	for (int threads = 1; threads <= 16; threads *= 2)
	{
		RunBenchmark(
			/*producers*/ threads,
			/*consumers*/ threads,
			/*tasksPerProducer*/ 1000000);
	}
}