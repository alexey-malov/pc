#include <future>
#include <iostream>
#include <stdexcept>
#include <syncstream>

int main()
{
	auto f1 = std::async(std::launch::async, [] { return 10; }).share();
	auto f2 = std::async(std::launch::async, [] { return 20; }).share();
	auto f3 = std::async(std::launch::async, //
		[](std::shared_future<int> x, std::shared_future<int> y) { //
			return x.get() + y.get();
		},
		f1, f2)
				  .share();

	auto f4 = std::async(std::launch::async, //
		[](std::shared_future<int> x, std::shared_future<int> y) { //
			return x.get() * y.get();
		},
		f2, f3);
	std::cout << f3.get() << ", " << f4.get();
}