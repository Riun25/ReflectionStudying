#pragma once
#include "Resolver.h"

/* �߰��� Ÿ��------------------------------
* type*, std::shared_ptr<>, std::unique_ptr<>
-------------------------------------------*/

namespace reflect
{
	// A type descriptor for unique_ptr<>
	class TypeDescriptor_StdUniquePtr : public TypeDescriptor
	{
	private:
		TypeDescriptor* itemType; // �����ڿ��� �ʱ�ȭ���ֱ� ���� ���� ����
		// �Լ� �����͸� �������� ���� ���� ����
		const void* (*GetItem)(const void*);
		void (*DeleteFunc)(void*); // ���� �Լ� ������ �߰�

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
			DeleteFunc(_obj); // ����� ���� �Լ� ȣ��
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

	//�Ϲ����� T�� �ƴ϶� std::unique_ptr<T>�� ���� ���Ǵ� Ư��ȭ�� ����
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