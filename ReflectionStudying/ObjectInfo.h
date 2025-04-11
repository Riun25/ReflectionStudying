#pragma once
#include <atomic>
struct ObjectInfo
{
	std::atomic<bool> isMarked = false;
	std::string TypeName = "";

	// 생성자
	ObjectInfo(bool _isMarked, const std::string& _TypeName)
		: isMarked(_isMarked), TypeName(_TypeName)
	{	}

	// 복사 생성자
	ObjectInfo(const ObjectInfo& _other)
		: isMarked(_other.isMarked.load()), TypeName(_other.TypeName)
	{	}

	// 복사 대입 연산자
	ObjectInfo& operator=(const ObjectInfo& _other)
	{
		if (this != &_other)
		{
			isMarked.store(_other.isMarked.load());
			TypeName = _other.TypeName;
		}
		return *this;
	}

	// 이동 생성자
	ObjectInfo(ObjectInfo&& _other) noexcept
		: isMarked(_other.isMarked.load()), TypeName(std::move(_other.TypeName))
	{
		_other.isMarked.store(false);
	}

	// 이동 대입 연산자
	ObjectInfo& operator=(ObjectInfo&& _other) noexcept
	{
		if (this != &_other)
		{
			isMarked.store(_other.isMarked.load());
			TypeName = std::move(_other.TypeName);
			_other.isMarked.store(false);
		}
		return *this;
	}
};
