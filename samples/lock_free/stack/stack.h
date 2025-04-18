#pragma once
#include <atomic>
#include <memory>

template <typename T>
class Stack
{
public:
	Stack() = default;
	Stack(const Stack&) = delete;
	Stack& operator=(const Stack&) = delete;
	~Stack()
	{
		while (m_head)
			delete std::exchange(m_head, m_head->next);
	}

	void Push(const T& data)
	{
		m_head = new Node{ std::make_unique<T>(data), m_head };
	}

	void Push(T&& data)
	{
		m_head = new Node{ std::make_unique<T>(std::move(data)), m_head };
	}

	std::unique_ptr<T> Pop() noexcept
	{
		std::unique_ptr<T> data;
		if (auto oldHead = m_head)
		{
			data = std::move(m_head->data);
			m_head = m_head->next;
			delete oldHead;
		}
		return data;
	}

private:
	struct Node
	{
		std::unique_ptr<T> data;
		Node* next;
	};

	Node* m_head = nullptr;
};
