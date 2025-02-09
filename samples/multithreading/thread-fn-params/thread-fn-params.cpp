#include <iostream>
#include <memory>
#include <string>
#include <thread>

using namespace std::literals;

void ByVal(std::string str)
{
	std::cout << "ByVal: " << str << std::endl;
}

void PassArgByValue()
{
	std::string param = "Hello";
	// Строка param будет скопирована во временное хранилище перед вызовом функции
	std::jthread t{ ByVal, param };
}

void ByRef(const std::string& str)
{
	std::cout << "ByRef: &str: " << &str << ": " << str << std::endl;
}

void PassArgByReference()
{
	std::string param = "Hello";
	std::cout << "&param: " << &param << std::endl;
	std::jthread t{ ByRef, std::cref(param) };
}

void DanglingReference()
{
	{
		char buffer[] = "This string is allocated on stack";
		std::thread{ ByRef, buffer }.detach();
		// При выходе из блока buffer будет разрушен.
		// Если фоновый поток в этот момент ещё не стартует,
		// возможна передача невалидного указателя в функцию ByRef
	}
	std::this_thread::sleep_for(1s);
}

class Spy
{
public:
	Spy()
	{
		std::cout << "Spy() in thread " << std::this_thread::get_id() << "\n";
	}
	Spy(int value)
	{
		std::cout << "Spy(" << value << ") in thread "
				  << std::this_thread::get_id() << "\n";
	}
	Spy(const Spy&)
	{
		std::cout << "Spy(const Spy&) in thread " << std::this_thread::get_id() << "\n";
	}
	Spy(Spy&&) noexcept
	{
		std::cout << "Spy(Spy&&) in thread " << std::this_thread::get_id() << "\n";
	}
	~Spy()
	{
		std::cout << "~Spy() in thread " << std::this_thread::get_id() << "\n";
	}
};

void ArgumentLifetimes()
{
	std::cout << "Main thread id: " << std::this_thread::get_id() << "\n";
	std::jthread t1{
		[](Spy) {
			std::cout << "Thread func is invoked in thread "
					  << std::this_thread::get_id() << "\n";
		},
		Spy()
	};
	t1.join();

	std::jthread t2{
		[](const Spy&) {
			std::cout << "Thread func is invoked in thread "
					  << std::this_thread::get_id() << "\n";
		},
		42
	};
	t2.join();
}

#if 0
void PassingStringLiteral()
{
	// Указатель на строковый литерал "Hello" будет скопирован внутрь потока
	// В новом потоке из этого указателя будет создана строка, ссылка на которую
	// будет передана в Func.
	// Так как строковый литерал имеет статическое время жизни,
	// указатель на него останется валиден в момент создания строки.
	std::jthread t1{ Func, "Hello" };
}
#endif

void PossibleDanglingReference()
{
	char buffer[] = "Some temporary buffer";
	std::thread(
		[](const std::string& str) {
			std::cout << "Dangling reference " << str << std::endl;
		},
		/* const char* */ buffer) // В thread будет сохранён адрес массива buffer
		.detach(); //
	// Массив buffer будет разрушен при выходе из функции.
	// При вызове лямбда функции в фоновом потоке ей будет
	// передан указатель на разрушенный массив
}

void NoDanglingRerefernce()
{
	{
		char buffer[] = "Some temporary buffer";
		std::thread(
			[](const std::string& str) {
				std::cout << str << std::endl;
			},
			std::string(buffer)) // Внутри потока будет сохранена копия строки
			.detach();
	}
	std::this_thread::sleep_for(1s);
}

int main()
{
	PassArgByValue();
	PassArgByReference();
	ArgumentLifetimes();
	NoDanglingRerefernce();
#if 0
	PossibleDanglingReference();
#endif
}