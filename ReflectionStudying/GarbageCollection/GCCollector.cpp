#include "GCCollector.h"

void GC::GarbageCollector::RegisterType(reflect::TypeDescriptor* typeDesc)
{
	registeredTypes.insert(typeDesc);
}

void GC::GarbageCollector::Allocate(void* _obj, reflect::TypeDescriptor* _typeDesc)
{
	heap[_obj] = _typeDesc; // ��ü�� Ÿ�� ��ũ���� ����
}

void GC::GarbageCollector::AddRoot(void* _root)
{
	roots.insert(_root); // ��Ʈ �߰�
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
	/// [��������] std::cout << "\nStarting Marking...\n";
	for (void* root : roots) 
	{
		if (markedObjects.find(root) == markedObjects.end()) 
		{
			auto it = heap.find(root);
			if (it != heap.end()) 
			{
				reflect::TypeDescriptor* typeDesc = it->second;
				typeDesc->Mark(root, markedObjects); // ���÷��� ������ ����Ͽ� ��ŷ ����
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
			typeDesc->Delete(it->first); // ���÷��� ������ ����Ͽ� ��ü ����
			it = heap.erase(it);         // ������ ����
		}
		else 
		{
			++it;
		}
	}
}

void GC::GarbageCollector::Collect()
{
	/// [��������] std::cout << "\nStarting Garbage Collection...\n";
	Mark();
	Sweep();
	/// [��������] std::cout << "Garbage Collection Complete.\n\n";
}
