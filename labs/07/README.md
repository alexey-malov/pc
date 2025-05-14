# Корутины

- [Корутины](#корутины)
  - [Задания](#задания)
    - [Требования](#требования)
    - [Задание 1 — Знакомство с корутинами — 30 баллов](#задание-1--знакомство-с-корутинами--30-баллов)
    - [Задание 2 — Генератор глав книг — 20 баллов](#задание-2--генератор-глав-книг--20-баллов)
    - [Задание 3 — Simple Awaiter — 30 баллов](#задание-3--simple-awaiter--30-баллов)

## Задания

- Для получения оценки "удовлетворительно" нужно набрать не менее x баллов.
- На оценку "хорошо" нужно набрать не менее y баллов.
- Для получения оценки "отлично" нужно набрать не менее z баллов.

### Требования

Обязательно проверяйте успешность всех вызовов функций операционной системы и используемых библиотек
и не игнорируйте ошибки.

Ваш код должен иметь уровень безопасности исключений не ниже базового.
Для этого разработайте (или возьмите готовую) RAII-обёртку, автоматизирующую
управление ресурсами операционной системы.

### Задание 1 — Знакомство с корутинами — 30 баллов

Реализуйте класс `MyTask`, который можно использовать для возврата строковых значений из корутины с помощью `co_return`.

Примечание: корутина не должна приостанавливаться при запуске.

```cpp
MyTask SimpleCoroutine()
{
    co_return "Hello from coroutine!";
}

int main()
{
    MyTask task = SimpleCoroutine();
    // Должно вывести "Hello from coroutine!"
    std::cout << task.GetResult() << std::endl;
}
```

### Задание 2 — Генератор глав книг — 20 баллов

Ознакомьтесь с классом `std::generator` (требуется поддержка c++ 23) (или воспользуйтесь его аналогом),
чтобы написать корутину, генерирующую главы книг.

```cpp
struct Book
{
    std::string title;
    std::string author;
    std::vector<std::string> chapters;
};

struct BookChapter
{
    std::string bookTitle;
    std::string bookAuthor;
    std::string chapterTitle;
};

std::ostream& operator<<(std::ostream& os, const BookChapter& chapter);

std::generator<BookChapter> ListBookChapters(const std::vector<Book>& chapters);

int main()
{
    std::vector<Book> books = {
        { "The Great Gatsby", "F. Scott Fitzgerald", { "Chapter 1", "Chapter 2" } },
        { "1984", "George Orwell", { "Chapter 1", "Chapter 2", "Chapter 3" } },
        { "To Kill a Mockingbird", "Harper Lee", { "Chapter 1" } }
    };

    for (const auto& chapter : ListBookChapters(books))
    {
        std::cout << chapter << std::endl;
    }
}
```

### Задание 3 — Simple Awaiter — 30 баллов

Напишите класс (или структуру) `MyAwaiter`, который можно использовать в качестве аргумента оператора `co_await`.
Также напишите класс `MyTask`, который можно возвращать из корутины.

```txt
Before await
Before resume
42
After await
After resume
```

```cpp
struct MyAwaiter
{
    /* Напишите тело структуры */
};

class MyTask
{
    /* Напишите тело класса */
};

MyTask CoroutineWithAwait(int x, int y)
{
    std::cout << "Before await\n";
    // Приостанавливает работу. При возобновлении возвращает сумму аргументов
    int result = co_await MyAwaiter{ x, y };
    std::cout << result << "\n";
    std::cout << "After await\n";
}

int main()
{
    auto task = CoroutineWithAwait(30, 12);
    std::cout << "Before resume\n";
    task.Resume(); // Возобновляем работу Task
    std::cout << "After resume\n";
    CoroutineWithAwait(5, 10).Resume();
    std::cout << "End of main\n";
}
```

Эта программа должна вывести следующий текст:

```txt
Before await
Before resume
42
After await
After resume
Before await
15
After await
End of main
```
