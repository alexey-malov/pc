#include "MeasureTime.h"
#include <iostream>
#include <memory>

class Data
{
public:
	Data()
	{
	}

	void UseData() const
	{
	}
};

class Widget
{
public:
	Widget() = default;
	Widget(const Widget&) = delete;
	Widget& operator=(const Widget&) = delete;
	~Widget()
	{
		delete m_data;
	}

	void Foo() const
	{
		if (!m_data)
		{
			m_data = new Data();
		}
		m_data->UseData();
	}

private:
	mutable const Data* m_data = nullptr;
};

void TestSingleThreaded()
{
	Widget widget;
	const unsigned numRounds = 8;
	for (unsigned r = 0; r < numRounds; ++r)
	{
		for (int i = 0; i < 1'000'000'000; ++i)
			widget.Foo();
	}
}

int main()
{
	for (int i = 0; i < 10; ++i)
	{
		MeasureTime("DCL (wrong) version", TestSingleThreaded);
	}
}
