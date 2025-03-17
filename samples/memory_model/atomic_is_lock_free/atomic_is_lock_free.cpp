#include <atomic>
#include <iomanip>
#include <iostream>

struct Point
{
	int x, y;
};

struct alignas(16) LargeType
{
	uint64_t a;
	uint64_t b;
};

int main()
{
	std::cout << std::boolalpha;
	std::cout << "atomic<int> is always lock free: "
			  << std::atomic<int>::is_always_lock_free << "\n";
	std::cout << "atomic<Point> is always lock free: "
			  << std::atomic<Point>::is_always_lock_free << "\n";
	std::atomic<LargeType> l;
	std::cout << "atomic<LargeType> is lock free: "
			  << l.is_lock_free() << "\n";
}
