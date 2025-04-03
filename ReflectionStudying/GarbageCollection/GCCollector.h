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

		// 객체 등록
		void Allocate(void* _obj, reflect::TypeDescriptor* _typeDesc);

		// 루트 관리
		void AddRoot(void* _root);
		void RemoveRoot(void* _root);
		void ClearRoots();

		// 가비지 컬렉션
		void Mark();
		void ConcurrentMark();
		void Sweep(bool _isDump);
		void Collect(bool _isDump = false);

		// 테스트를 위한 heap size
		inline int GetHeapSize() { return heap.size(); }

	private:
		std::unordered_set<void*> roots; // 루트 집합
		std::unordered_set<const void*> markedObjects; // 마킹된 객체 집합
		std::unordered_map<void*, reflect::TypeDescriptor*> heap;     // 객체와 타입 정보
		std::unordered_set<reflect::TypeDescriptor*> registeredTypes; // 등록된 타입 정보
	};
}