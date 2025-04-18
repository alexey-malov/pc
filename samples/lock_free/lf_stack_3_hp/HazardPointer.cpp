#include "HazardPointer.h"

namespace
{
constexpr unsigned MaxHazardPointers = 100;
HazardPointer HazardPointers[MaxHazardPointers];

// Указатель на список узлов для отложенного удаления
std::atomic<DataToReclaim*> NodesToReclaim = nullptr;

} // namespace

HpOwner::HpOwner()
	: m_hp(nullptr)
{
	const auto thisThreadId = std::this_thread::get_id();

	for (auto& hp : HazardPointers)
	{
		std::thread::id nullId; // id, не соответствующий никакому потоку
		if (hp.id.compare_exchange_strong(nullId, thisThreadId))
		{
			// Свободный слот найден, сохраняем адрес на hazard pointer
			m_hp = &hp;
			break;
		}
	}

	if (!m_hp)
	{
		// Слишком много потоков используют hazard pointer-ы
		throw std::runtime_error("No hazard pointers available");
	}
}

bool OutstandingHazardPointersFor(void* p) noexcept
{
	for (auto& hp : HazardPointers)
	{
		if (hp.pointer.load() == p)
		{
			return true;
		}
	}
	return false;
}

std::atomic<void*>& GetHazardPointerForCurrentThread()
{
	// При первом вызове этой функции для потока будет создан его hazard pointer
	thread_local static HpOwner hazard;

	// При следующих вызовах будет возвращаться закешированное значение указателя
	return hazard.GetPointer();
}

// Добавляем узел для будущего освобождения
void AddToReclaimList(DataToReclaim* node) noexcept
{
	node->next = NodesToReclaim.load();
	while (!NodesToReclaim.compare_exchange_weak(node->next, node))
		;
}

void DeleteNodesWithNoHazards() noexcept
{
	// Пытаемся забрать себе список узлов для удаления
	auto current = NodesToReclaim.exchange(nullptr);
	while (current)
	{
		DataToReclaim* const next = current->next;
		if (!OutstandingHazardPointersFor(current->data))
		{
			// Удаляем узел вместе с его данными
			delete current;
		}
		else
		{
			// Указатель current->data удалять опасно, добавляем current
			// в список для последующего удаления
			AddToReclaimList(current);
		}
		current = next;
	}
}
