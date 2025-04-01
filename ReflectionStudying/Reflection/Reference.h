#pragma once
#include "Resolver.h"

/* 추가한 타입------------------------------
* type*, std::shared_ptr<>, std::unique_ptr<>
-------------------------------------------*/

namespace reflect
{
	// A type descriptor for unique_ptr<>
	class TypeDescriptor_StdUniquePtr : public TypeDescriptor
	{
	private:
		TypeDescriptor* itemType; // 생성자에서 초기화해주기 위한 내부 선언
		// 함수 포인터를 가져오기 위한 변수 선언
		const void* (*GetItem)(const void*);
		void (*DeleteFunc)(void*); // 삭제 함수 포인터 추가

	public:
		virtual std::string GetFullName() const override
		{
			return std::string("std::unique_ptr<" + itemType->GetFullName() + ">");
		}
		virtual void Dump(const void* _obj, int _indentLevel) const override
		{
			const void* item = GetItem(_obj);
			std::cout << GetFullName() << " {";

			if (item != nullptr)
			{
				itemType->Dump(GetItem(_obj), _indentLevel + 1);
			}
			else
			{
				std::cout << "nullptr";
			}
			std::cout << " }";
		}

		virtual void Mark(const void* _obj, std::unordered_set<const void*>& _markedObjects) const override
		{
			const void* item = GetItem(_obj);
			if (item != nullptr)
			{
				itemType->Mark(item, _markedObjects);
			}
		}

		virtual void Delete(void* _obj) const override
		{
			if (_obj == nullptr)
			{
				return;
			}
			DeleteFunc(_obj); // 저장된 삭제 함수 호출
		}

		template <typename ItemType>
		TypeDescriptor_StdUniquePtr(ItemType*)
			:TypeDescriptor("std::unique_ptr<>", sizeof(std::unique_ptr<ItemType>)), itemType(TTypeResolver<ItemType>::Get())
		{
			GetItem = [](const void* _pUnique) -> const void*
				{
					const auto& _pUnique = *(const std::unique_ptr<ItemType>*) _pUnique;
					return _pUnique;
				};

			DeleteFunc = [](void* _obj)
				{
					std::unique_ptr<ItemType> ptr(static_cast<ItemType*>(_obj));
					ptr.reset();
				};
		}
	};

	//일반적인 T가 아니라 std::unique_ptr<T>일 때만 사용되는 특수화된 버전
	template <typename T>
	class TTypeResolver<std::unique_ptr<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdUniquePtr typeDesc{ (T*) nullptr };
			return &typeDesc;
		}
	};
}