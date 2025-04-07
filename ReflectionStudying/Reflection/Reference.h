#pragma once
#include "Resolver.h"

/* �߰��� Ÿ��----------------------------------------------------
* raw_ptr, std::shared_ptr<>, std::unique_ptr<>, std::weak_ptr<>
-----------------------------------------------------------------*/

namespace reflect
{
	/*------------------------------------
	  A type descriptor for raw_ptr
	-------------------------------------*/
	class TypeDescriptor_RawPtr : public TypeDescriptor
	{
	private:
		TypeDescriptor* itemType;
		const void* (*GetItem)(const void*);

	public:
		virtual std::string GetFullName() const override
		{
			return std::string("raw_ptr<" + itemType->GetFullName() + ">");
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

		template <typename ItemType>
		TypeDescriptor_RawPtr(ItemType*)
			:TypeDescriptor("std::raw_ptr<>", sizeof(ItemType)), itemType(TTypeResolver<ItemType>::Get())
		{
			GetItem = [](const void* _ptr) -> const void*
				{
					return *(ItemType**)_ptr;
				};
		}
	};

	template <typename T>
	class TTypeResolver<T*>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_RawPtr typeDesc((T*)nullptr);
			return &typeDesc;
		}
	};

	/*------------------------------------
	  A type descriptor for unique_ptr<>
	 -------------------------------------*/
	class TypeDescriptor_StdUniquePtr : public TypeDescriptor
	{
	private:
		TypeDescriptor* itemType; // �����ڿ��� �ʱ�ȭ���ֱ� ���� ���� ����
		// �Լ� �����͸� �������� ���� ���� ����
		const void* (*GetItem)(const void*);

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

		template <typename ItemType>
		TypeDescriptor_StdUniquePtr(ItemType*)
			:TypeDescriptor("std::unique_ptr<>", sizeof(std::unique_ptr<ItemType>)), itemType(TTypeResolver<ItemType>::Get())
		{
			GetItem = [](const void* _ptr) -> const void*
				{
					const auto& _ptr = *(const std::unique_ptr<ItemType>*) _ptr;
					return _ptr.get();
				};
		};
	};

	//�Ϲ����� T�� �ƴ϶� std::unique_ptr<T>�� ���� ���Ǵ� Ư��ȭ�� ����
	template <typename T>
	class TTypeResolver<std::unique_ptr<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdUniquePtr typeDesc((T*) nullptr);
			return &typeDesc;
		}
	};

	/*------------------------------------
	  A type descriptor for shared_ptr<>
	 -------------------------------------*/
	class TypeDescriptor_StdSharedPtr : public TypeDescriptor
	{
	private:
		TypeDescriptor* itemType; // �����ڿ��� �ʱ�ȭ���ֱ� ���� ���� ����
		// �Լ� �����͸� �������� ���� ���� ����
		const void* (*GetItem)(const void*);

	public:
		virtual std::string GetFullName() const override
		{
			return std::string("std::shared_ptr<" + itemType->GetFullName() + ">");
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

		template <typename ItemType>
		TypeDescriptor_StdSharedPtr(ItemType*)
			:TypeDescriptor("std::shared_ptr<>", sizeof(std::shared_ptr<ItemType>)), itemType(TTypeResolver<ItemType>::Get())
		{
			GetItem = [](const void* _ptr) -> const void*
				{
					const auto& _ptr = *(const std::shared_ptr<ItemType>*) _ptr;
					return _ptr.get();
				};
		}
	};

	//�Ϲ����� T�� �ƴ϶� std::shared_ptr<T>�� ���� ���Ǵ� Ư��ȭ�� ����
	template <typename T>
	class TTypeResolver<std::shared_ptr<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdSharedPtr typeDesc((T*)nullptr);
			return &typeDesc;
		}
	};

	/*------------------------------------
	  A type descriptor for weak_ptr<>
	-------------------------------------*/
	class TypeDescriptor_StdWeakPtr : public TypeDescriptor
	{
	private:
		TypeDescriptor* itemType; // �����ڿ��� �ʱ�ȭ���ֱ� ���� ���� ����
		// �Լ� �����͸� �������� ���� ���� ����
		const void* (*GetItem)(const void*);

	public:
		virtual std::string GetFullName() const override
		{
			return std::string("std::weak_ptr<" + itemType->GetFullName() + ">");
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

		template <typename ItemType>
		TypeDescriptor_StdWeakPtr(ItemType*)
			:TypeDescriptor("std::weak_ptr<>", sizeof(std::weak_ptr<ItemType>)), itemType(TTypeResolver<ItemType>::Get())
		{
			GetItem = [](const void* _ptr) -> const void*
				{
					const auto& _ptr = *(const std::weak_ptr<ItemType>*) _ptr;
					//weak_ptr�� ���� �޸𸮸� ������ �ʿ� ������, expired()�� ���� ���� Ȯ���� �ʿ�.
					if (_ptr.expired() == true) 
					{
						return nullptr;
					}
					else 
					{
						return _ptr.lock().get();
					}
				};
		}
	};

	//�Ϲ����� T�� �ƴ϶� std::weak_ptr<T>�� ���� ���Ǵ� Ư��ȭ�� ����
	template <typename T>
	class TTypeResolver<std::weak_ptr<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdSharedPtr typeDesc((T*)nullptr);
			return &typeDesc;
		}
	};
}