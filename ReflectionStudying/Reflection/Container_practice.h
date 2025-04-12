#pragma once
#include "Resolver.h"

/* 추가한 타입------------------------------
std::vector<>, std::map< , >, std::array
-------------------------------------------*/

namespace reflect
{
	// 모든 vector 타입의 공통 베이스
	class TypeDescriptor_Vector : public TypeDescriptor
	{
	public:
		TypeDescriptor_Vector(const char* _name, size_t _size)
			: TypeDescriptor(_name, _size) {}

		virtual size_t GetSize(const void*) const = 0;
		virtual const void* GetItem(const void*, size_t) const = 0;
		virtual bool IsPrimitive() const override { return true; }
	};

	template<typename ItemType>
	class TypeDescriptor_StdVector : public TypeDescriptor_Vector
	{
	public:
		explicit TypeDescriptor_StdVector(ItemType* = nullptr)
			: TypeDescriptor_Vector("std::vector<>", sizeof(std::vector<ItemType>))  // 기반 클래스 생성자
		{
			itemType = TTypeResolver<ItemType>::Get();
		}

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

		virtual size_t GetSize(const void* obj) const
		{
			return static_cast<const std::vector<ItemType>*>(obj)->size();
		}

		virtual const void* GetItem(const void* _vecPtr, size_t _index) const override
		{
			const auto& _vec = *(const std::vector<ItemType>*) _vecPtr;
			if constexpr (std::is_pointer<ItemType>::value)
			{
				return _vec[_index];
			}
			else
			{
				return &_vec[_index];
			}
		}
	public:
		TypeDescriptor* itemType;
	};


	////A type descriptor for vector<>
	//class TypeDescriptor_StdVector : public TypeDescriptor
	//{
	//public:
	//	TypeDescriptor* itemType; // 생성자에서 초기화해주기 위한 내부 선언
	//	// 함수 포인터를 가져오기 위한 변수 선언
	//	size_t(*GetSize)(const void*);
	//	const void* (*GetItem)(const void*, size_t);

	//public:
	//	virtual std::string GetFullName() const override
	//	{
	//		return std::string("std::vector<" + itemType->GetFullName() + ">");
	//	}
	//	virtual void Dump(const void* _obj, int _indentLevel) const override
	//	{
	//		size_t numItems = GetSize(_obj);
	//		std::cout << GetFullName();
	//		if (numItems == 0)
	//		{
	//			std::cout << "{}";
	//		}
	//		else
	//		{
	//			std::cout << name << "\n" << std::string(4 * _indentLevel, ' ') << "{\n";
	//			for (size_t i = 0; i < numItems; ++i)
	//			{
	//				std::cout << std::string(4 * (_indentLevel + 1), ' ') << "[" << i << "]";
	//				itemType->Dump(GetItem(_obj, i), _indentLevel + 1);
	//				std::cout << "\n";
	//			}
	//			std::cout << std::string(4 * _indentLevel, ' ') << "}";
	//		}
	//	}

	//	virtual bool IsPrimitive() const override { return true; }



	//	template <typename ItemType>
	//	TypeDescriptor_StdVector(ItemType*)
	//		:TypeDescriptor("std::vector<>", sizeof(std::vector<ItemType>)), itemType(TTypeResolver<ItemType>::Get())
	//	{
	//		GetSize = [](const void* _vecPtr) -> size_t
	//			{
	//				// 간단한 디버그 출력
	//				const auto& vec = *(const std::vector<ItemType>*) _vecPtr;
	//				//std::cout << "Vec addr (container): " << _vecPtr << std::endl;
	//				//std::cout << "Vec data addr       : " << vec.data() << std::endl;
	//				//std::cout << "Vec size            : " << vec.size() << std::endl;
	//				return vec.size();
	//			};
	//		GetItem = [](const void* _vecPtr, size_t _index) -> const void*
	//			{
	//				const auto& _vec = *(const std::vector<ItemType>*) _vecPtr;
	//				if constexpr (std::is_pointer<ItemType>::value)
	//				{
	//					return _vec[_index];
	//				}
	//				else
	//				{
	//					return &_vec[_index];
	//				}
	//			};
	//	}
	//};

	template<typename T>
	TypeDescriptor_StdVector(T*) -> TypeDescriptor_StdVector<T>;

	//일반적인 T가 아니라 std::vector<T>일 때만 사용되는 특수화된 버전
	template <typename T>
	class TTypeResolver<std::vector<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdVector<T> typeDesc{ nullptr };
			return &typeDesc;
		}
	};
}


////--------------------------------
//void ObjectManager::Mark(const std::string& _typeName, ObjectInfo* _obj)
//{
//	if (_obj->isMarked == true) return; // 이미 마킹됨
//
//	_obj->isMarked = true; // 마킹 수행
//	++markedCount;
//
//	auto* typeDesc = FindTypeDescriptor(_obj->TypeName);
//
//	if (typeDesc == nullptr) { return; }
//
//	for (const auto& member : typeDesc->memberVec)
//	{
//		//auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
//		auto* vectorType = dynamic_cast<reflect::TypeDescriptor_Vector*>(member.type);
//		if (vectorType != nullptr) // 벡터 타입이라면 재귀적으로 진행
//		{
//			void* memberPtr = static_cast<char*>(static_cast<void*>(_obj)) + member.offset;
//
//			// 벡터 데이터 접근
//			size_t numItems = vectorType->GetSize(memberPtr);
//			for (size_t i = 0; i < numItems; ++i)
//			{
//				ObjectInfo* child = static_cast<ObjectInfo*>(const_cast<void*>(vectorType->GetItem(memberPtr, i)));
//
//				if (vectorType->IsPrimitive() == false) // Primitive 타입 무시
//				{
//					Mark(_obj->TypeName, child);
//				}
//			}
//		}
//	}
//}
///------------