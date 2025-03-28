#include "../Reflection/Reflect.h"
#include "GCCollector.h"

void GC::GarbageCollector::Allocate(void* _obj)
{
    heap.insert(_obj);
}

void GC::GarbageCollector::AddRoot(void* _root)
{
    roots.push_back(_root);
    std::cout << "Object added as a root.\n";
}

void GC::GarbageCollector::ClearRoots()
{
    roots.clear();
}

void GC::GarbageCollector::Mark(reflect::TypeDescriptor* _typeDesc)
{
	std::cout << "\nStarting Marking...\n";

    for (void* root : roots)
    {
        _typeDesc->Mark(root, markedObjects);
    }
}

void GC::GarbageCollector::Sweep(reflect::TypeDescriptor* _typeDesc)
{
    std::cout << "\nStarting Sweeping...\n";
    for (auto it = heap.begin(); it != heap.end();)
    {
        if (markedObjects.find(*it) == markedObjects.end()) // ��ŷ���� ���� ��� ����
        {
            std::cout << "Collecting: " << *it << "\n";
            //delete static_cast<_typeDesc.name>(*it); TODO :: �о�� Ŭ���� �̸����� ��ü�� ��
            _typeDesc->Delete(*it);
            it = heap.erase(it);
        }
        else
        {
            ++it;
        }
    }
    markedObjects.clear(); // ���� GC�� ���� �ʱ�ȭ
}

void GC::GarbageCollector::Collect(reflect::TypeDescriptor* _typeDesc)
{
    std::cout << "\nStarting Garbage Collection...\n";
    Mark(_typeDesc);
    Sweep(_typeDesc);
    std::cout << "Garbage Collection Complete.\n\n";
}
