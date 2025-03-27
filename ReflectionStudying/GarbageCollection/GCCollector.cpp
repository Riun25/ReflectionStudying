#include "GCCollector.h"

GC::Object* GC::GarbageCollector::CreateObject()
{
    Object* obj = new Object(nextID++);
    heap.insert(obj);
    std::cout << "Created Object " << obj->id << "\n";
    return obj;
}

void GC::GarbageCollector::AddRoot(Object* _obj)
{
    roots.push_back(_obj);
    std::cout << "Object " << _obj->id << " added as a root.\n";
}

void GC::GarbageCollector::mark(Object* _obj)
{
    if (_obj == nullptr || _obj->marked == true)
    {
        return;
    }
    _obj->marked = true;
    std::cout << "Marked Object " << _obj->id << "\n";
    for (Object* ref : _obj->references)
    {
        mark(ref);
    }
}

void GC::GarbageCollector::Sweep()
{
    std::cout << "Sweeping...\n";
    for (auto it = heap.begin(); it != heap.end();)
    {
        if ((*it)->marked == false)
        {
            std::cout << "Deleting Object " << (*it)->id << "\n";
            delete* it;
            it = heap.erase(it);
        }
        else
        {
            (*it)->marked = false;
            ++it;
        }
    }
}

void GC::GarbageCollector::Collect()
{
    std::cout << "\nStarting Garbage Collection...\n";
    for (Object* root : roots)
    {
        mark(root);
    }
    Sweep();
    std::cout << "Garbage Collection Complete.\n\n";
}
