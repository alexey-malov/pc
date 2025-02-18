#include <chrono>
#include <iostream>
#include <thread>

using namespace std::literals;

int main()
{
	int value = 1; // A

	std::jthread t1{
		[&value] { // B
			value += 1; // C
		} // D
	}; // E
	std::jthread t2{
		[&t1, &value] { // F
			std::jthread t3{
				[&value] { // G
					std::this_thread::sleep_for(1s); // H
					value *= 3; // I
				} // J
			}; // K
			t1.join(); // L
			value = value * value; // M
		} // N
	}; // O
	t2.join(); // P
	std::cout << value << "\n"; // Q
}
