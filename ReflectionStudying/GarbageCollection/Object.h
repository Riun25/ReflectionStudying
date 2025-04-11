#pragma once
#include <atomic>
#include "../Reflection/Container.h"
#include "../Reflection/ReflectMacro.h"

// �̻�� ����ü
struct ObjectSlot
{
	void* object;
	mutable std::atomic_flag isMarkedFlag = ATOMIC_FLAG_INIT;
	//bool isMarked;
	reflect::TypeDescriptor* type;

	ObjectSlot() = default;

	ObjectSlot(void* _obj, reflect::TypeDescriptor* _typeDesc)
		: object(_obj), type(_typeDesc) {}

	// ���� ���� (�⺻ ����)
	ObjectSlot(const ObjectSlot&) = default;
	ObjectSlot& operator=(const ObjectSlot&) = default;

	ObjectSlot(ObjectSlot&& _other) noexcept
		: object(_other.object), type(_other.type)
	{
		// ��ŷ ���´� �̵� ��󿡼��� �ʱ�ȭ��
		isMarkedFlag.clear(std::memory_order_relaxed);

		_other.object = nullptr;
		_other.isMarkedFlag.clear(std::memory_order_relaxed);
		_other.type = nullptr;
	}

	ObjectSlot& operator=(ObjectSlot&& _other) noexcept
	{
		if (this != &_other)
		{
			object = _other.object;
			type = _other.type;

			isMarkedFlag.clear(std::memory_order_relaxed);
			_other.object = nullptr;
			_other.type = nullptr;
			_other.isMarkedFlag.clear(std::memory_order_relaxed);

		}
		return *this;
	}

	// ��ŷ �õ�: ó���̸� true ��ȯ, �̹� ��ŷ�Ǿ� ������ false
	bool TryMark() const
	{
		return !isMarkedFlag.test_and_set(std::memory_order_relaxed);
	}

	// GC ������ ��ŷ �ʱ�ȭ�� �� ���
	void ClearMark()
	{
		isMarkedFlag.clear(std::memory_order_relaxed);
	}

	bool IsMarked() const
	{
		return isMarkedFlag.test(std::memory_order_relaxed);
	}

};