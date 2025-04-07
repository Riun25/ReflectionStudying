#pragma once
#include "Resolver.h"

/* 추가한 타입------------------------------
std::vector<>, std::map< , >, std::array
-------------------------------------------*/

namespace reflect
{
	//A type descriptor for vector<>
	class TypeDescriptor_StdVector : public TypeDescriptor
	{
	public:
		TypeDescriptor* itemType; // 생성자에서 초기화해주기 위한 내부 선언
		// 함수 포인터를 가져오기 위한 변수 선언
		size_t(*GetSize)(const void*);
		const void* (*GetItem)(const void*, size_t);

	public:
		virtual std::string GetFullName() const override
		{
			return std::string("std::vector<" + itemType->GetFullName() + ">");
		}
		virtual void Dump(const void* _obj, int _indentLevel) const override
		{
			size_t numItems = GetSize(_obj);
			std::cout << GetFullName();
			if (numItems == 0)
			{
				std::cout << "{}";
			}
			else
			{
				std::cout << name << "\n" << std::string(4 * _indentLevel, ' ') << "{\n";
				for (size_t i = 0; i < numItems; ++i)
				{
					std::cout << std::string(4 * (_indentLevel + 1), ' ') << "[" << i << "]";
					itemType->Dump(GetItem(_obj, i), _indentLevel + 1);
					std::cout << "\n";
				}
				std::cout << std::string(4 * _indentLevel, ' ') << "}";
			}
		}

		virtual bool IsPrimitive() const override { return true; }

		template <typename ItemType>
		TypeDescriptor_StdVector(ItemType*)
			:TypeDescriptor("std::vector<>", sizeof(std::vector<ItemType>)), itemType(TTypeResolver<ItemType>::Get())
		{
			GetSize = [](const void* _vecPtr) -> size_t
				{
					const auto& vec = *(const std::vector<ItemType>*) _vecPtr;
					return vec.size();
				};
			GetItem = [](const void* _vecPtr, size_t _index) -> const void*
				{
					const auto& _vec = *(const std::vector<ItemType>*) _vecPtr;
					return &_vec[_index];
				};
		}
	};

	//일반적인 T가 아니라 std::vector<T>일 때만 사용되는 특수화된 버전
	template <typename T>
	class TTypeResolver<std::vector<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdVector typeDesc{ (T*) nullptr };
			return &typeDesc;
		}
	};
}