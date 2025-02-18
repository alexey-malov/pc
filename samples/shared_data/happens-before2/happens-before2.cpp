#include <iostream>
#include <thread>

int main()
{
	int value = 0; // A

	std::jthread t1{
		[&value] { // B
			value += 1; // C
		} // D
	}; // E

	std::jthread t2{
		[&t1, &value] { // F
			t1.join(); // G
			value *= 2; // H
		} // I
	}; // J
	t2.join(); // K
	std::cout << value << "\n"; // L
}
