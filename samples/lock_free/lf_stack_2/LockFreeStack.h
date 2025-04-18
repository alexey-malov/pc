#pragma once
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
		DeleteNodes(m_nodesToDelete.load());
		DeleteNodes(m_head);
	}

	void Push(const T& data)
	{
		Push(new Node(data));
	}

	void Push(T&& data)
	{
		Push(new Node(std::move(data)));
	}

	std::unique_ptr<T> Pop() noexcept
	{
		++m_threadsInPop; // Увеличиваем счётчик потоков, делающих Pop
		Node* oldHead = m_head.load();
		while (oldHead != nullptr && !m_head.compare_exchange_weak(oldHead, oldHead->next))
			;
		auto data = oldHead ? std::move(oldHead->data) : nullptr;
		TryReclaim(oldHead); // Помещаем узел в список на удаление
		return data;
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

	void Push(Node* newNode) noexcept
	{
		newNode->next = m_head.load();
		// Пытаемся заменить головной узел, пока не преуспеем
		while (!m_head.compare_exchange_weak(/*expected*/ newNode->next, /*desired*/ newNode))
			/*если мы здесь, значит другому потоку повезло*/;
		// После выхода из цикла, m_head будет равен newNode
	}

	void TryReclaim(Node* oldHead) noexcept
	{
		if (m_threadsInPop == 1) // Мы единственный поток внутри Pop?
		{
			// Пытаемся забрать список узлов себе
			Node* nodesToDelete = m_nodesToDelete.exchange(nullptr);
			if (--m_threadsInPop == 0) // Текущий поток всё ещё последний?
			{
				// Больше никто не работает со списком узлов, поэтому его можно удалить
				DeleteNodes(nodesToDelete);
			}
			else if (nodesToDelete != nullptr)
			{ // Кто-то ещё работает со списком узлов - добавляем узлы в список
				ChainPendingNodes(nodesToDelete);
			}
			delete oldHead; // Узел можно удалить, так только текущий поток извлёк этот узел
		}
		else
		{
			if (oldHead)
				ChainPendingNode(oldHead);
			--m_threadsInPop;
		}
	}

	static void DeleteNodes(Node* nodes) noexcept
	{
		while (nodes)
			delete std::exchange(nodes, nodes->next);
	}

	void ChainPendingNodes(Node* const first) noexcept
	{
		assert(first);
		Node* last = first;
		while (Node* const next = last->next)
			last = next;
		ChainPendingNodes(first, last);
	}

	// Дописываем диапазон узлов [first, last] в начало списка
	void ChainPendingNodes(Node* first, Node* last) noexcept
	{
		assert(first && last);
		last->next = m_nodesToDelete;
		// Цикл нужен, так как другие потоки тоже могут выполнять это же действие
		while (!m_nodesToDelete.compare_exchange_weak(last->next, first))
			;
	}

	void ChainPendingNode(Node* node) noexcept
	{
		assert(node);
		ChainPendingNodes(node, node);
	}

	std::atomic<Node*> m_head = nullptr;
	std::atomic<unsigned> m_threadsInPop = 0;
	std::atomic<Node*> m_nodesToDelete = nullptr;
};
