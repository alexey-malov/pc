# Защита данных с помощью мьютексов

- [Защита данных с помощью мьютексов](#защита-данных-с-помощью-мьютексов)
  - [Задания](#задания)
    - [Требования](#требования)
    - [Задание №1 — 100 баллов](#задание-1--100-баллов)
      - [Бонус за добавление дополнительных персонажей — 30 баллов](#бонус-за-добавление-дополнительных-персонажей--30-баллов)
      - [Бонус за тонкое использование блокировок — 50 баллов](#бонус-за-тонкое-использование-блокировок--50-баллов)
    - [Задание №2](#задание-2)
      - [Вариант 1 — Многопоточный склад — 30 баллов](#вариант-1--многопоточный-склад--30-баллов)

## Задания

- Для получения оценки "удовлетворительно" нужно набрать не менее 90 баллов.
- На оценку "хорошо" нужно набрать не менее 150 баллов.
- Для получения оценки "отлично" нужно набрать не менее 200 баллов.

### Требования

Обязательно проверяйте успешность всех вызовов функций операционной системы и не оставляйте ошибки незамеченными.

Ваш код должен иметь уровень безопасности исключений не ниже базового.
Для этого разработайте (или возьмите готовую) RAII-обёртку, автоматизирующую
управление ресурсами операционной системы.

### Задание №1 — 100 баллов

Напишите программу, которая эмулирует работу экономики страны.

Изначально в экономике находится некоторое количество наличных денег,
распределённых между её гражданами в некоторой пропорции (например, равномерно).

Граждане могут открывать и закрывать счёт в банке, помещать деньги на своё счёт (Deposit)
и снимать деньги со своего счёта (Withdraw).
Банк учитывает количество денег в наличном обороте при снятии денег и помещении на счёт.
Поэтому нельзя поместить деньги на счёт больше, чем имеется в наличии.

Можно переводить деньги со своего счёта на счёт другого человека.
Перевод денег должен выполняться транзакционно: если некоторая сумма была зачислена на счёт получателя,
то эта же сумма должна быть списана со счёта отправителя.
В случае ошибок перевод должен откатываться.

Для этого разработайте класс `Bank` со следующими методами:

```cpp
using AccountId = unsigned long long;
using Money = long long;

class BankOperationError : std::runtime_error
{
public:
  using runtime_error::runtime_error;
};

// Контролирует все деньги в обороте (как наличные, так и безналичные)
class Bank
{
public:
  // Инициализирует монетарную систему. cash — количество денег в наличном обороте
  // При отрицательном количестве денег, выбрасывается BankOperationError
  explicit Bank(Money cash);

  Bank(const Bank&) = delete;
  Bank& operator=(const Bank&) = delete;

  // Возвращает количество операций, выполненных банком (включая операции чтения состояния)
  // Для неблокирующего подсчёта операций используйте класс std::atomic<unsigned long long>
  // Вызов метода GetOperationsCount() не должен участвовать в подсчёте
  unsigned long long GetOperationsCount() const;

  // Перевести деньги с исходного счёта (srcAccountId) на целевой (dstAccountId)
  // Нельзя перевести больше, чем есть на исходном счёте
  // Нельзя перевести отрицательное количество денег
  // Исключение BankOperationError выбрасывается, при отсутствии счетов или
  // недостатке денег на исходном счёте
  // При отрицательном количестве переводимых денег выбрасывается std::out_of_range
  void SendMoney(AccountId srcAccountId, AccountId dstAccountId, Money amount);

  // Перевести деньги с исходного счёта (srcAccountId) на целевой (dstAccountId)
  // Нельзя перевести больше, чем есть на исходном счёте
  // Нельзя перевести отрицательное количество денег
  // При нехватке денег на исходном счёте возвращается false
  // Если номера счетов невалидны, выбрасывается BankOperationError
  // При отрицательном количестве денег выбрасывается std::out_of_range
  [[nodiscard]] bool TrySendMoney(AccountId srcAccountId, AccountId dstAccountId, Money amount);

  // Возвращает количество наличных денег в обороте
  [[nodiscard]] Money GetCash() const;

  // Сообщает о количестве денег на указанном счёте
  // Если указанный счёт отсутствует, выбрасывается исключение BankOperationError
  Money GetAccountBalance(AccountId accountId) const;

  // Снимает деньги со счёта. Нельзя снять больше, чем есть на счете
  // Нельзя снять отрицательное количество денег
  // Снятые деньги переходят добавляются к массе наличных денег
  // При невалидном номере счёта или отсутствии денег, выбрасывается исключение BankOperationError
  // При отрицательном количестве денег выбрасывается std::out_of_range
  void WithdrawMoney(AccountId account, Money amount);

  // Попытаться снять деньги в размере amount со счёта account.
  // Объем денег в наличном обороте увеличивается на величину amount
  // При нехватке денег на счёте возвращается false, а количество наличных денег остаётся неизменным
  // При невалидном номере аккаунта выбрасывается BankOperationError.
  // При отрицательном количестве денег выбрасывается std::out_of_range
  [[nodiscard]] bool TryWithdrawMoney(AccountId account, Money amount);

  // Поместить наличные деньги на счёт. Количество денег в наличном обороте
  // уменьшается на величину amount.
  // Нельзя поместить больше, чем имеется денег в наличном обороте
  // Нельзя поместить на счёт отрицательное количество денег
  // Нельзя поместить деньги на отсутствующий счёт
  // При невалидном номере аккаунта или нехватке наличных денег в обороте выбрасывается BankOperationError.
  // При отрицательном количестве денег выбрасывается std::out_of_range
  void DepositMoney(AccountId account, Money amount);

  // Открывает счёт в банке. После открытия счёта на нём нулевой баланс.
  // Каждый открытый счёт имеет уникальный номер.
  // Возвращает номер счёта
  [[nodiscard]] AccountId OpenAccount();

  // Закрывает указанный счёт.
  // Возвращает количество денег, которые были на счёте в момент закрытия
  // Эти деньги переходят в наличный оборот
  [[nodiscard]] Money CloseAccount(AccountId accountId);
};
```

Для класса `Bank` должны быть разработаны юнит тесты.

**Класс `Bank` должен быть потокобезопасным**: его методы должно быть возможно безопасно
вызывать из разных потоков.

Также методы класса `Bank` должны быть **безопасными с точки зрения исключений**:
предоставлять гарантию безопасности не ниже базовой.

Во время сдачи лабораторных работу нужно обосновать обеспечение описанных гарантий безопасности.

Разработайте приложение, в котором ряд [акторов](https://ru.wikipedia.org/wiki/%D0%90%D0%BA%D1%82%D0%BE%D1%80_(%D1%81%D0%BE%D1%86%D0%B8%D0%B0%D0%BB%D1%8C%D0%BD%D1%8B%D0%B5_%D0%BD%D0%B0%D1%83%D0%BA%D0%B8))
участвуют в экономике.
Каждый из акторов на очередном шаге симуляции выполняет действия в соответствии со следующим сценарием:

- Гомер, отец семейства. Гомер работает на электростанции.
  Он регулярно переводит некоторую сумму денег своей жене Мардж. Перевод осуществляет путём перевода денег на карту Мардж.
  Некоторую сумму денег Гомер платит за электричество в доме.
  
  Также Гомер регулярно снимает наличные деньги со счёта и даёт их своим детям: Барту и Лизе.

  ![Homer](images/homer.png)
- Мардж, жена Гомера, домохозяйка. Покупает продукты у Апу, владельца супермаркета.
  При покупке переводит деньги на счёт Апу в банке.

  ![Marge](images/marge.png)
- Барт и Лиза, когда у них есть наличные деньги, тратят их небольшими порциями на покупку
  товаров у Апу.
  У них нет банковской карты, поэтому они расплачиваются с Апу наличкой.

  ![Bart](images/bart.png)![Lisa](images/lisa.png)
- Апу, владелец супермаркета. Регулярно платит за электроэнергию электростанции,
  на которой работает Гомер.
  Когда у Апу появляются наличные деньги, они помещает их на свой счёт в банке.

  ![Apu](images/apu.png)
- Владелец электростанции Мистер Бернс, на которой работает Гомер, регулярно платит ему зарплату из тех денег,
  которые перечисляют ему за электричество.

  ![Burns](images/burns.png)

В процессе симуляции возможна ситуация, когда у кого-то из участников не будет достаточного количества денег.
В этом случае он должен пропустить ход.

Приложение должно поддерживать два режима симуляции:

- Однопоточный. В этом режиме все участники выполняют свои действия последовательно в одном потоке.
- Параллельный. Персонажи действуют параллельно друг другу, каждый в своём потоке.

В процессе симуляции в stdout должна выводиться диагностическая информация о действиях, выполняемых участниками.
Вывод информации должно быть возможно отключить программно, чтобы замерить пропускную способность системы.

Симуляция заканчивается спустя некоторое время, передаваемого приложению с командной строки,
**либо путём отправки сигналов SIGINT либо SIGTERM**.
Параллельно работающие потоки должны завершить свою работу. В программе не должно быть дедлоков.
Приложение должно вывести количество выполненных банковских операций, а также удостовериться,
что количество банковская система находится в согласованном состоянии:

- Сумма наличных денег у персонажей совпадает с суммой наличных денег, зарегистрированных в банке.
- Общая сумма всех наличных и безналичных денег на счетах равна сумме, которая изначально была положена в банк в виде наличных.

#### Бонус за добавление дополнительных персонажей — 30 баллов

- Хулиган Нельсон. Время от времени втайне ворует случайную сумму наличных денег у Барта.
  Покупает на эти деньги сигареты у Апу.
  
  ![Nelson](images/nelson.png)

- Преступник Честер "Змей" Турли. Время от времени взламывает компьютер Гомера
  и переводит некоторую сумму денег с его счёта на свой. Покупает продукты у Апу.

  ![Snake](images/snake.png)

- Вейлон Смиттерс. Личный помощник владельца электростанции. Получает деньги за свою работу от владельца электростанции.
  Тратит их на покупку продуктов в супермаркете. Оплачивает деньги переводом.
  Параноик, поэтому время от времени закрывает и заново открывает счёт в банке.
  Не всегда вовремя уведомляет владельца электростанции о смене счёта, поэтому
  возможна ситуация,
  при которой владелец электростанции пытается перевести деньги на ставший невалидным счёт,
  и перевод заканчивается неудачей.

  ![Smitters](images/smitters.png)

Добавление этих новых персонажей должно также сохранять общее количество денег в системе.
В параллельном режиме работы для каждого их этих персонажей создаётся отдельный поток.

#### Бонус за тонкое использование блокировок — 50 баллов

Чтобы повысить пропускную способность системы используйте не один глобальный мьютекс на всю систему, а несколько мьютексов:

- Перевод денег между непересекающимися счетами должны быть способны выполняться без блокировки.
- Непересекающийся обмен (или кража) наличных денег должны быть способны выполняться без блокировки.
- Во время открытия и закрытия счёта возможна остановка банковской системы.

### Задание №2

#### Вариант 1 — Многопоточный склад — 30 баллов

Разработайте потокобезопасный класс `Warehouse`, который хранит определённое количество единиц товара.
Потоки-поставщики периодически привозят некоторое количество товара на склад,
потоки-покупатели периодически забирают некоторое количество товара, если есть товары.
Потоки, отвечающие за инвентаризацию, периодически выводят количество имеющихся единиц товара на складе.

Синтаксис командной строки:

```bash
warehouse NUM_SUPPLIERS NUM_CLIENTS NUM_AUDITORS
```

Склад имеет определённую вместимость: добавить больше товара, чем имеется нельзя.
Поэтому поток-поставщик, должен повторить попытку размещения товара через некоторое время.
Потоки-покупатель не может забрать больше товара, чем имеется.

Программу должно быть возможно прервать при помощи сигналов SIGINT и SIGTERM.
Перед выходом программа потоки-поставщики должны вывести общее количество помещённых на склад товаров,
потоки покупатели — общее количество приобретенных товаров, а главный поток — количество оставшихся товаров на складе.
