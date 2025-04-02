#pragma once
#include <unordered_set>
#include <vector>
#include <iostream>
#include "../Reflection/Descriptor.h"

class reflect::TypeDescriptor;

namespace GC
{
	class GarbageCollector
	{
	public:
		GarbageCollector() {}
		~GarbageCollector() {}

	public:
		static GarbageCollector& GetInstance() 
		{
			static GarbageCollector instance;
			return instance;
		}

	public:
		void RegisterType(reflect::TypeDescriptor* typeDesc);

		// ��ü ���
		void Allocate(void* _obj, reflect::TypeDescriptor* _typeDesc);

		// ��Ʈ ����
		void AddRoot(void* _root);
		void RemoveRoot(void* _root);
		void ClearRoots();

		// ������ �÷���
		void Mark();
		void Sweep();
		void Collect();

		// �׽�Ʈ�� ���� heap publicȭ
		std::unordered_map<void*, reflect::TypeDescriptor*> heap;     // ��ü�� Ÿ�� ����

	private:
		std::unordered_set<void*> roots; // ��Ʈ ����
		std::unordered_set<const void*> markedObjects; // ��ŷ�� ��ü ����
		std::unordered_set<reflect::TypeDescriptor*> registeredTypes; // ��ϵ� Ÿ�� ����
	};
}