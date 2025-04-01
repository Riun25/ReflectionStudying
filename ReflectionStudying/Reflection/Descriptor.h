#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <any>

namespace reflect
{
	class TypeDescriptor
	{
	public:
		TypeDescriptor() : name(nullptr), size(0) {}
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



	class TypeDescriptor_SubClass :public TypeDescriptor
	{
	public:
		struct Member
		{
			std::string name;
			size_t offset;
			TypeDescriptor* type; // 타입 정보를 저장 (예: int, std::string 등)
		};

		struct Function
		{
			std::string name;
			std::vector<std::string> parameterTypes; // 매개변수 타입 정보 저장 -> 검증용이지 실제로는 없어도 됨
			std::function<void(void*, const std::vector<std::any>&)> invoker;

			Function(const std::string& name, const std::vector<std::string>& parameterTypes,
				const std::function<void(void*, const std::vector<std::any>&)>& invoker)
				: name(name), parameterTypes(parameterTypes), invoker(invoker) {
			}
		};

	public: // TODO : 공란이니 채울 것
		void IncrementReference()
		{
			referenceCount++;
		}

		void DecrementReference()
		{
			if (--referenceCount == 0)
			{
				Delete(this);
			}
		}

		int GetReferenceCount() const
		{
			return referenceCount;
		}
		virtual void Dump(const void* _obj, int _indentLevel = 0) const override
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
					<< ", Parameters: (";
				for (const auto& paramType : eFunction.parameterTypes)
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

		// 초기화 함수 포인터
		using InitFunc = void(*)(TypeDescriptor_SubClass*);
		TypeDescriptor_SubClass(InitFunc initFunc) { initFunc(this); }

	private:
		int referenceCount = 0;
	};
}