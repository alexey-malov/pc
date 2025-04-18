#pragma once
#include <atomic>
#include <functional>
#include <stdexcept>
#include <thread>

struct HazardPointer
{
	std::atomic<std::thread::id> id;
	std::atomic<void*> pointer;
};

class HpOwner
{
public:
	HpOwner(const HpOwner&) = delete;
	HpOwner& operator=(const HpOwner&) = delete;

	HpOwner();

	std::atomic<void*>& GetPointer() noexcept
	{
		return m_hp->pointer;
	}

	~HpOwner()
	{
		// Освобождаем место в массиве hazard pointer-ов
		m_hp->pointer.store(nullptr);
		m_hp->id.store(std::thread::id());
	}

private:
	HazardPointer* m_hp;
};

// Есть ли среди HazardPointers указатель, равный p?
bool OutstandingHazardPointersFor(void* p) noexcept;

std::atomic<void*>& GetHazardPointerForCurrentThread();

struct DataToReclaim
{
	void* data;
	std::function<void(void*)> deleter;
	DataToReclaim* next = nullptr;

	// Сохраняет данные, подлежащие удалению, в виде void*
	template <typename T>
	DataToReclaim(T* p)
		: data(p)
		, deleter([](void* p) { delete static_cast<T*>(p); })
	{
	}

	~DataToReclaim()
	{
		// Предполагается, что deleter не будет бросать исключений,
		deleter(data);
	}
};

// Добавляем узел для будущего освобождения
void AddToReclaimList(DataToReclaim* node) noexcept;

void DeleteNodesWithNoHazards() noexcept;

template <typename T>
void ReclaimLater(T* data)
{
	AddToReclaimList(new DataToReclaim(data));
}