#include "GCCollector.h"

void GC::GarbageCollector::RegisterType(reflect::TypeDescriptor* typeDesc)
{
	registeredTypes.insert(typeDesc);
}

void GC::GarbageCollector::Allocate(void* _obj, reflect::TypeDescriptor* _typeDesc)
{
	heap[_obj] = _typeDesc; // 객체와 타입 디스크립터 저장
}

void GC::GarbageCollector::AddRoot(void* _root)
{
	roots.insert(_root); // 루트 추가
}

void GC::GarbageCollector::RemoveRoot(void* _root)
{
	roots.erase(_root);
}

void GC::GarbageCollector::ClearRoots()
{
	roots.clear();
}

void GC::GarbageCollector::Mark()
{
	/// [성능측정] std::cout << "\nStarting Marking...\n";
	for (void* root : roots) 
	{
		if (markedObjects.find(root) == markedObjects.end()) 
		{
			auto it = heap.find(root);
			if (it != heap.end()) 
			{
				reflect::TypeDescriptor* typeDesc = it->second;
				typeDesc->Mark(root, markedObjects); // 리플렉션 정보를 사용하여 마킹 수행
			}
		}
	}
}

void GC::GarbageCollector::Sweep()
{
	for (auto it = heap.begin(); it != heap.end(); ) 
	{
		if (markedObjects.find(it->first) == markedObjects.end()) 
		{
			std::cout << "Collecting: " << it->first << "\n";
			reflect::TypeDescriptor* typeDesc = it->second;
			typeDesc->Delete(it->first); // 리플렉션 정보를 사용하여 객체 삭제
			it = heap.erase(it);         // 힙에서 제거
		}
		else 
		{
			++it;
		}
	}
}

void GC::GarbageCollector::Collect()
{
	/// [성능측정] std::cout << "\nStarting Garbage Collection...\n";
	Mark();
	Sweep();
	/// [성능측정] std::cout << "Garbage Collection Complete.\n\n";
}
