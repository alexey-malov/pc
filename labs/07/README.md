# Корутины

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
