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

	//  �⺻ Ÿ�Կ� ���� TypeDescriptor ��ü�� ��ȯ
	template <typename T>
	TypeDescriptor* GetPrimitiveDescriptor();



	class TypeDescriptor_SubClass :public TypeDescriptor
	{
	public:
		struct Member
		{
			std::string name;
			size_t offset;
			TypeDescriptor* type; // Ÿ�� ������ ���� (��: int, std::string ��)
		};

		struct Function
		{
			std::string name;
			std::vector<std::string> parameterTypes; // �Ű����� Ÿ�� ���� ���� -> ���������� �����δ� ��� ��
			std::function<void(void*, const std::vector<std::any>&)> invoker;

			Function(const std::string& name, const std::vector<std::string>& parameterTypes,
				const std::function<void(void*, const std::vector<std::any>&)>& invoker)
				: name(name), parameterTypes(parameterTypes), invoker(invoker) {
			}
		};

	public: // TODO : �����̴� ä�� ��
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
				// std::cout << "Marking object: " << (char*)_obj << "\n"; ������
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

			// ��� ���� ����
			for (const Member& member : memberVec)
			{
				void* memberPtr = (char*)_obj + member.offset;
				member.type->Delete(memberPtr);  // ����� Ÿ�� ������ ����� ����
			}
		}
	public:
		std::vector<Member> memberVec;
		std::vector<Function> functionVec;

		// �ʱ�ȭ �Լ� ������
		using InitFunc = void(*)(TypeDescriptor_SubClass*);
		TypeDescriptor_SubClass(InitFunc initFunc) { initFunc(this); }

	private:
		int referenceCount = 0;
	};
}