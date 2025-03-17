#include <iostream>
#include <memory>

struct Data
{
	int i;
	double d;
	unsigned bf1 : 10;
	int bf2 : 12;
	int /*bf3*/ : 0;
	int bf4 : 9;
	int i2;
	char c1, c2;
	std::string s;
};

int main()
{
	std::cout << sizeof(Data) << "\n";
}
