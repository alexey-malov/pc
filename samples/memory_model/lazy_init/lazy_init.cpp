#include <iostream>
#include <memory>

class Data
{
public:
	Data()
	{
		std::cout << "Data::Data()\n";
	}

	void UseData() const
	{
		std::cout << "Data::UseData()\n";
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
		std::cout << "Widget::Foo()\n";
		if (!m_data)
		{
			m_data = new Data();
		}
		m_data->UseData();
	}

private:
	mutable const Data* m_data = nullptr;
};

int main()
{
	Widget widget;
	widget.Foo();
	widget.Foo();
}
