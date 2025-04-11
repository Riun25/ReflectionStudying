#pragma once
#include <atomic>
struct ObjectInfo
{
	std::atomic<bool> isMarked = false;
	std::string TypeName = "";

	// ������
	ObjectInfo(bool _isMarked, const std::string& _TypeName)
		: isMarked(_isMarked), TypeName(_TypeName)
	{	}

	// ���� ������
	ObjectInfo(const ObjectInfo& _other)
		: isMarked(_other.isMarked.load()), TypeName(_other.TypeName)
	{	}

	// ���� ���� ������
	ObjectInfo& operator=(const ObjectInfo& _other)
	{
		if (this != &_other)
		{
			isMarked.store(_other.isMarked.load());
			TypeName = _other.TypeName;
		}
		return *this;
	}

	// �̵� ������
	ObjectInfo(ObjectInfo&& _other) noexcept
		: isMarked(_other.isMarked.load()), TypeName(std::move(_other.TypeName))
	{
		_other.isMarked.store(false);
	}

	// �̵� ���� ������
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
