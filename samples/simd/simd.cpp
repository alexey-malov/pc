#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

class Timer
{
	using Clock = std::chrono::high_resolution_clock;

public:
	explicit Timer(std::string name)
		: m_name(std::move(name))
	{
	}

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	~Timer()
	{
		Stop();
	}

	void Stop()
	{
		if (m_running)
		{
			m_running = false;
			auto cur = Clock::now();
			auto dur = cur - m_start;
			std::cout << m_name << " took " << dur.count() << "ns (" << std::chrono::duration<double>(dur).count()
					  << "s)" << std::endl;
		}
	}

private:
	std::string m_name;
	Clock::time_point m_start = Clock::now();
	bool m_running = true;
};

template <typename Fn, typename... Args>
decltype(auto) MeasureTime(std::string name, Fn&& fn, Args&&... args)
{
	Timer t{ std::move(name) };
	return std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
}

struct alignas(32) Vector4d
{
	double items[4];
};

void ArrayTransform(const Vector4d* src1, const Vector4d* src2, Vector4d* dst, double scale, int size)
{
	for (int i = 0; i < size; ++i)
	{
		dst[i].items[0] = src1[i].items[0] * scale + src2[i].items[0];
		dst[i].items[1] = src1[i].items[1] * scale + src2[i].items[1];
		dst[i].items[2] = src1[i].items[2] * scale + src2[i].items[2];
		dst[i].items[3] = src1[i].items[3] * scale + src2[i].items[3];
	}
}

struct Vector4du
{
	double items[4];
};

void ArrayTransform(const Vector4du* src1, const Vector4du* src2, Vector4du* dst, double scale, int size)
{
	for (int i = 0; i < size; ++i)
	{
		dst[i].items[0] = src1[i].items[0] * scale + src2[i].items[0];
		dst[i].items[1] = src1[i].items[1] * scale + src2[i].items[1];
		dst[i].items[2] = src1[i].items[2] * scale + src2[i].items[2];
		dst[i].items[3] = src1[i].items[3] * scale + src2[i].items[3];
	}
}

int main()
{
	constexpr size_t numItems = 10'000;
	constexpr size_t numIterations = 100'000;

	{
		std::vector<Vector4d> vec1(numItems);
		std::vector<Vector4d> vec2(numItems);
		std::vector<Vector4d> dst(numItems);

		// Пробегаемся по массиву вхолостую, чтобы прогреть кеш
		ArrayTransform(vec1.data(), vec2.data(), dst.data(), 42.0, numItems);
		MeasureTime("Aligned sum", [&vec1, &vec2, &dst] {
			for (size_t i = 0; i < numIterations; ++i)
			{
				ArrayTransform(vec1.data(), vec2.data(), dst.data(), 42.0, numItems);
			}
		});
	}

	{
		std::vector<Vector4du> vec1u(numItems);
		std::vector<Vector4du> vec2u(numItems);
		std::vector<Vector4du> dstu(numItems);

		// Пробегаемся по массиву вхолостую, чтобы прогреть кеш
		ArrayTransform(vec1u.data(), vec2u.data(), dstu.data(), 42.0, numItems);
		MeasureTime("Unaligned sum", [&vec1u, &vec2u, &dstu] {
			for (size_t i = 0; i < numIterations; ++i)
			{
				ArrayTransform(vec1u.data(), vec2u.data(), dstu.data(), 42.0, numItems);
			}
		});
	}
}