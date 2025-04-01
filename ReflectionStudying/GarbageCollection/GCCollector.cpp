#include "GCCollector.h"

void GC::GarbageCollector::Allocate(void* _obj)
{
    heap.insert(_obj);
}

void GC::GarbageCollector::AddRoot(void* _root)
{
	if (std::find(roots.begin(), roots.end(), _root) == roots.end()) 
    {
		roots.push_back(_root);
		static_cast<reflect::TypeDescriptor_SubClass*>(_root)->IncrementReference();
		std::cout << "Object added as a root.\n";
	}
	else 
    {
		std::cout << "Object is already a root.\n";
	}
}

void GC::GarbageCollector::RemoveRoot(void* _root)
{
    auto it = std::find(roots.begin(), roots.end(), _root);
    if (it != roots.end())
    {
        roots.erase(it);
    	static_cast<reflect::TypeDescriptor_SubClass*>(_root)->DecrementReference();
	    std::cout << "Object removed as a root.\n";
    }
    else
    {
        std::cout << "Object not found in roots.\n";
    }
}

void GC::GarbageCollector::ClearRoots()
{
    roots.clear();
}

void GC::GarbageCollector::Mark(reflect::TypeDescriptor* _typeDesc)
{
    /// [성능측정] std::cout << "\nStarting Marking...\n";

    for (void* root : roots)
    {
        _typeDesc->Mark(root, markedObjects);
    }
}

void GC::GarbageCollector::Sweep(reflect::TypeDescriptor* _typeDesc)
{
    /// [성능측정]std::cout << "\nStarting Sweeping...\n";
    for (auto it = heap.begin(); it != heap.end();)
    {
        auto* obj = static_cast<reflect::TypeDescriptor_SubClass*>(*it);
        if (markedObjects.find(*it) == markedObjects.end() && obj->GetReferenceCount() == 0) // 마킹되지 않은 경우 삭제
        {
            std::cout << "Collecting: " << *it << "\n";
            _typeDesc->Delete(*it);
            it = heap.erase(it);
        }
        else
        {
            ++it;
        }
    }
    markedObjects.clear(); // 다음 GC를 위해 초기화
}

void GC::GarbageCollector::Collect(reflect::TypeDescriptor* _typeDesc)
{
    /// [성능측정] std::cout << "\nStarting Garbage Collection...\n";
    Mark(_typeDesc);
    Sweep(_typeDesc);
    /// [성능측정] std::cout << "Garbage Collection Complete.\n\n";
}
