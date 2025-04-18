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
		for (auto h = m_head.load(); h;)
			h = h->next;
	}

	void Push(const T& data)
	{
		PushImpl(data);
	}

	void Push(T&& data)
	{
		PushImpl(std::move(data));
	}

	std::unique_ptr<T> Pop() noexcept
	{
		auto oldHead = m_head.load();
		while (oldHead != nullptr
			&& !m_head.compare_exchange_weak(oldHead, oldHead->next))
			;
		if (oldHead)
		{
			oldHead->next = std::shared_ptr<Node>();
			return std::move(oldHead->data);
		}
		return {};
	}

private:
	struct Node
	{
		std::unique_ptr<T> data;
		std::shared_ptr<Node> next;

		template <typename U>
		Node(U&& d)
			: data(std::make_unique<T>(std::forward<U>(d)))
		{
		}
	};

	template <typename U>
	void PushImpl(U&& data)
	{
		std::shared_ptr<Node> const newNode = std::make_shared<Node>(std::forward<U>(data));
		newNode->next = m_head.load();
		// Пытаемся заменить головной узел, пока не преуспеем
		while (!m_head.compare_exchange_weak(/*expected*/ newNode->next, /*desired*/ newNode))
			;
	}

	std::atomic<std::shared_ptr<Node>> m_head = nullptr;
};
