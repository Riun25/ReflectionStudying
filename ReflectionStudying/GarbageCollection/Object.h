#pragma once
#include <atomic>
#include "../Reflection/Container.h"
#include "../Reflection/ReflectMacro.h"

// 미사용 구조체
struct ObjectSlot
{
	void* object;
	mutable std::atomic_flag isMarkedFlag = ATOMIC_FLAG_INIT;
	//bool isMarked;
	reflect::TypeDescriptor* type;

	ObjectSlot() = default;

	ObjectSlot(void* _obj, reflect::TypeDescriptor* _typeDesc)
		: object(_obj), type(_typeDesc) {}

	// 복사 가능 (기본 동작)
	ObjectSlot(const ObjectSlot&) = default;
	ObjectSlot& operator=(const ObjectSlot&) = default;

	ObjectSlot(ObjectSlot&& _other) noexcept
		: object(_other.object), type(_other.type)
	{
		// 마킹 상태는 이동 대상에서는 초기화됨
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

	// 마킹 시도: 처음이면 true 반환, 이미 마킹되어 있으면 false
	bool TryMark() const
	{
		return !isMarkedFlag.test_and_set(std::memory_order_relaxed);
	}

	// GC 끝나고 마킹 초기화할 때 사용
	void ClearMark()
	{
		isMarkedFlag.clear(std::memory_order_relaxed);
	}

	bool IsMarked() const
	{
		return isMarkedFlag.test(std::memory_order_relaxed);
	}

};