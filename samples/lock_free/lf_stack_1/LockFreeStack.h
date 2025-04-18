#pragma once
#include <atomic>
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
		for (Node* h = m_head.load(); h;)
			delete std::exchange(h, h->next);
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
		Node* oldHead = m_head.load();
		while (oldHead != nullptr
			&& !m_head.compare_exchange_weak(oldHead, oldHead->next))
			;
		// Внимание, память, занимаемая oldHead утечёт
		return oldHead ? std::move(oldHead->data) : std::unique_ptr<T>();
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

	std::atomic<Node*> m_head = nullptr;
};
