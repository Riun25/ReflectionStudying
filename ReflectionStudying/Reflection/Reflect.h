#pragma once
#include <string>
#include <iostream>
#include <cstddef>
#include <vector>
#include <type_traits>
#include <unordered_set>

namespace reflect
{
	class TypeDescriptor
	{
	public:
		TypeDescriptor(const char* _name, size_t _size) : name(_name), size(_size) {}
		virtual ~TypeDescriptor() {}

	public:
		virtual std::string GetFullName() const { return name; }
		virtual void Dump(const void* _obj, int _indentLevel = 0) const = 0;
		virtual void Mark(const void* _obj, std::unordered_set<const void*>& _markedObjects) const = 0;
		virtual void Delete(void* _obj) const = 0;

	public:
		const char* name;
		size_t size;
	};

	//  기본 타입에 대한 TypeDescriptor 객체를 반환
	template <typename T>
	TypeDescriptor* GetPrimitiveDescriptor();

	// T의 static 멤버변수 Reflection의 유무에 따른 TypeDescriptor 객체 찾기
	class DefaultResolver
	{
	public:
		template <typename T>
		struct IsReflected
		{
			static constexpr bool value = requires {&T::Reflection; };
		};

		template <typename T>
		static constexpr auto Get() -> std::enable_if_t<IsReflected<T>::value, TypeDescriptor*>
		{
			return &T::Reflection;
		}

		template <typename T>
		static constexpr auto Get() -> std::enable_if_t<!IsReflected<T>::value, TypeDescriptor*>
		{
			return GetPrimitiveDescriptor<T>();
		}
	};

	template <typename T>
	class TTypeResolver
	{
	public:
		static TypeDescriptor* Get()
		{
			return DefaultResolver::Get<T>();
		}
	};

	class TypeDescriptor_SubClass : public TypeDescriptor
	{
	public:
		struct Member
		{
			const char* name;
			size_t offset;
			TypeDescriptor* type;
		};

		struct Function
		{
			const char* name;
			const char* returnType;
			std::vector<const char*> parameterTypes;
		};


	public:
		TypeDescriptor_SubClass(void (*init)(TypeDescriptor_SubClass*)) : TypeDescriptor(nullptr, 0)
		{
			init(this);
		}
		TypeDescriptor_SubClass(const char* _name, size_t _size, const std::initializer_list<Member>& _init) : TypeDescriptor(nullptr, 0), memberVec(_init) {}

	public:
		virtual void Dump(const void* _obj, int _indentLevel) const override
		{
			std::cout << name << "\n" << std::string(4 * _indentLevel, ' ') << "{\n";

			for (const Member& member : memberVec)
			{
				std::cout << std::string(4 * (_indentLevel + 1), ' ') << member.name << " = ";
				member.type->Dump((char*)_obj + member.offset, _indentLevel + 1);
				std::cout << "\n";
			}

			for (const Function& eFunction : functionVec)
			{
				std::cout << std::string(4 * (_indentLevel + 1), ' ') << "Function: " << eFunction.name
					<< " -> Return Type: " << eFunction.returnType << ", Parameters: (";
				for (const char* paramType : eFunction.parameterTypes)
				{
					std::cout << paramType << " ";
				}
				std::cout << ")\n";
			}

			std::cout << std::string(4 * _indentLevel, ' ') << "}\n";
		}

		virtual void Mark(const void* _obj, std::unordered_set<const void*>& _markedObjects) const override
		{
			if (_obj == nullptr)
			{
				return;
			}

			if (_markedObjects.find(_obj) != _markedObjects.end())
			{
				return;
			}
			_markedObjects.insert(_obj);
			
			for (const Member& member : memberVec)
			{
				// std::cout << "Marking object: " << (char*)_obj << "\n"; 디버깅용
				const void* memberPtr = (char*)_obj + member.offset;
				member.type->Mark(memberPtr, _markedObjects);
			}
		}

		virtual void Delete(void* _obj) const override
		{
			if (_obj == nullptr)
			{
				return;
			}

			// 멤버 변수 삭제
			for (const Member& member : memberVec) 
			{
				void* memberPtr = (char*)_obj + member.offset;
				member.type->Delete(memberPtr);  // 멤버의 타입 정보를 사용해 삭제
			}
		}


	public:
		std::vector<Member> memberVec;
		std::vector<Function> functionVec;
	};

#define REFLECT() \
	friend class reflect::DefaultResolver; \
	static reflect::TypeDescriptor_SubClass Reflection; \
	static void initReflection(reflect::TypeDescriptor_SubClass*);

#define REFLECT_STRUCT_BEGIN(_type) \
    reflect::TypeDescriptor_SubClass _type::Reflection{ _type::initReflection }; \
    void _type::initReflection(reflect::TypeDescriptor_SubClass* _typeDesc) \
    { \
        using T = _type; \
        _typeDesc->name = #_type; \
        _typeDesc->size = sizeof(T); \
        _typeDesc->memberVec.clear(); \
        _typeDesc->functionVec.clear();

#define REFLECT_STRUCT_MEMBER(_name) \
        _typeDesc->memberVec.push_back({#_name, offsetof(T, _name), reflect::TTypeResolver<decltype(T::_name)>::Get()});

#define REFLECT_STRUCT_FUNCTION(_name, _returnType, ...) \
        _typeDesc->functionVec.push_back({#_name, #_returnType, {#__VA_ARGS__}});

#define REFLECT_STRUCT_END() \
    }

	class TypeDescriptor_StdVector : public TypeDescriptor
	{
	private:
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