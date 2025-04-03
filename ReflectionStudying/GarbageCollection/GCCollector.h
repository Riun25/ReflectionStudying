#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
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
		void ConcurrentMark();
		void Sweep(bool _isDump);
		void Collect(bool _isDump = false);

		// �׽�Ʈ�� ���� heap size
		inline int GetHeapSize() { return heap.size(); }

	private:
		std::unordered_set<void*> roots; // ��Ʈ ����
		std::unordered_set<const void*> markedObjects; // ��ŷ�� ��ü ����
		std::unordered_map<void*, reflect::TypeDescriptor*> heap;     // ��ü�� Ÿ�� ����
		std::unordered_set<reflect::TypeDescriptor*> registeredTypes; // ��ϵ� Ÿ�� ����
	};
}