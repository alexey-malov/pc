#pragma once
#include "HazardPointer.h"
#include <atomic>
#include <cassert>
#include <memory>

template <typename T>
class LockFreeStack
{
public:
	LockFreeStack() = default;
	LockFreeStack(const LockFreeStack&) = delete;
	LockFreeStack& operator=(const LockFreeStack&) = delete;
	~LockFreeStack()
	{
		DeleteNodesWithNoHazards();
	}

	void Push(const T& data)
	{
		Push(new Node(data));
	}

	void Push(T&& data)
	{
		Push(new Node(std::move(data)));
	}

	std::unique_ptr<T> Pop()
	{
		Node* oldHead = TryExtractHead();

		return TryPopDataFromHead(oldHead);
	}

private:
	struct Node
	{
		std::unique_ptr<T> data;
		Node* next = nullptr;

		Node(T&& d)
			: data(std::make_unique<T>(std::move(d)))
		{
		}

		Node(const T& d)
			: data(std::make_unique<T>(d))
		{
		}
	};

	Node* TryExtractHead()
	{
		// Указатель опасности текущего потока
		std::atomic<void*>& hp = GetHazardPointerForCurrentThread();
		Node* oldHead = m_head.load(); // 1
		do
		{

			Node* temp;
			do // Устанавливаем hazard pointer на head.
			{
				temp = oldHead;
				// Сохраняем указатель на текущую голову стека в hazard pointer
				hp.store(oldHead); // 2

				// Здесь мы наивно полагаемся на то, что безопасно сравнивать указатель
				// на удалённый объект. Фактически, это UB, так как oldHead мог юыть удалён между (1) и (2)

				oldHead = m_head.load();
			} while (oldHead != temp);

			// Теперь можно безопасно разыменовывать oldHead, так как oldHead сохранён в hazard pointer
		} while (oldHead && !m_head.compare_exchange_strong(oldHead, oldHead->next));
		// Здесь используется compare_exchange_strong, чтобы не крутить лишний раз внутренний цикл
		// при spurious failure.

		hp.store(nullptr); // Очищаем hazard pointer, так как oldHead теперь у нас

		return oldHead;
	}

	static std::unique_ptr<T> TryPopDataFromHead(Node* oldHead)
	{
		std::unique_ptr<T> data;
		if (oldHead)
		{
			data = std::move(oldHead->data);
			// Есть ли hazard pointer-ы, ссылающиеся на oldHead?
			if (OutstandingHazardPointersFor(oldHead))
			{
				// Сейчас удалять голову небезопасно. Откладываем её удаление
				// Вызов ReclaimLater не безопасен относительно исключений,
				// подумайте, как это можно исправить
				ReclaimLater(oldHead);
			}
			else
			{
				delete oldHead; // Никто на узел не ссылается. Можно удалить его
			}
			// Удаляем узлы, на которые не ссылаются hazard pointer-ы
			DeleteNodesWithNoHazards();
		}

		return data;
	}

	void Push(Node* newNode) noexcept
	{
		newNode->next = m_head.load();
		// Пытаемся заменить головной узел, пока не преуспеем
		while (!m_head.compare_exchange_weak(/*expected*/ newNode->next, /*desired*/ newNode))
			/*если мы здесь, значит другому потоку повезло*/;
		// После выхода из цикла, m_head будет равен newNode
	}

	std::atomic<Node*> m_head = nullptr;
};
