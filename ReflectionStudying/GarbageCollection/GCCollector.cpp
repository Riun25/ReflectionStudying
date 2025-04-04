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

void GC::GarbageCollector::ConcurrentMark()
{
	//size_t numThreads = std::thread::hardware_concurrency();
	//markQueues.resize(numThreads);

	//// ��Ʈ�� �� �������� �۾� ť�� �й�
	//size_t index = 0;
	//for (void* root : roots)
	//{
	//	markQueues[index % numThreads].push(root);
	//	++index;
	//}

	//// �� �����忡�� ��ŷ �۾� ����
	//std::vector<std::thread> threads; // <- �� �̰� �ſ� ���е�... �Ź� �Ҵ�..?
	//for (size_t i = 0; i < numThreads; ++i)
	//{
	//	threads.emplace_back([this, i]() // <-�̰� ���� ��������?
	//		{
	//			while (markQueues[i].empty() == true)
	//			{
	//				void* obj = markQueues[i].front();
	//				markQueues[i].pop();

	//				// �̹� ��ŷ�� ��� �ǳʶ�
	//				{
	//					std::lock_guard<std::mutex> lock(markedObjectsMutex);
	//					if (markedObjects.find(obj) != markedObjects.end())
	//					{
	//						continue;
	//					}
	//					markedObjects.insert(obj);
	//				}

	//				// ���÷��� ������ ����Ͽ� ������ ��ü �߰�
	//				reflect::TypeDescriptor* typeDesc = heap[obj];
	//				typeDesc->Mark(obj, markedObjects, markQueues[i]);
	//			}
	//		});
	//}
	//for (auto& thread : threads)
	//{
	//	thread.join();
	//}

}

void GC::GarbageCollector::Sweep(bool _isDump)
{
	for (auto it = heap.begin(); it != heap.end(); ) 
	{
		if (markedObjects.find(it->first) == markedObjects.end()) 
		{
			if (_isDump == true)
			{
				std::cout << "Collecting: " << it->first << "\n";
			}
			reflect::TypeDescriptor* typeDesc = it->second;
			typeDesc->Delete(it->first); // ���÷��� ������ ����Ͽ� ��ü ����
			it = heap.erase(it);         // ������ ����
		}
		else 
		{
			++it;
		}
	}

	markedObjects.clear(); // ���� GC�� ���� �ʱ�ȭ
}

void GC::GarbageCollector::Collect(bool _isDump)
{
	/// [��������] std::cout << "\nStarting Garbage Collection...\n";
	Mark();
	//ConcurrentMark();
	Sweep(_isDump);
	/// [��������] std::cout << "Garbage Collection Complete.\n\n";
}
