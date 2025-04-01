#pragma once
#include "Resolver.h"

/* �߰��� Ÿ��------------------------------
std::vector<>, std::map< , >, std::array
-------------------------------------------*/

namespace reflect
{
	//A type descriptor for vector<>
	class TypeDescriptor_StdVector : public TypeDescriptor
	{
	private:
		TypeDescriptor* itemType; // �����ڿ��� �ʱ�ȭ���ֱ� ���� ���� ����
		// �Լ� �����͸� �������� ���� ���� ����
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

		virtual void Mark(const void* _obj, std::unordered_set<const void*>& _markedObjects) const override
		{
			size_t numItems = GetSize(_obj);
			for (size_t i = 0; i < numItems; ++i)
			{
				const void* item = GetItem(_obj, i);
				itemType->Mark(item, _markedObjects);
			}
		}

		virtual void Delete(void* _obj) const override
		{
			if (_obj == nullptr)
			{
				return;
			}

			size_t numItems = GetSize(_obj);
			for (size_t i = 0; i < numItems; ++i)
			{
				const void* item = GetItem(_obj, i);
				itemType->Delete(const_cast<void*>(item));
			}
		}

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

	//�Ϲ����� T�� �ƴ϶� std::vector<T>�� ���� ���Ǵ� Ư��ȭ�� ����
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