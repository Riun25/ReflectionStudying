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

void GC::GarbageCollector::ConcurrentMark()
{
	//size_t numThreads = std::thread::hardware_concurrency();
	//markQueues.resize(numThreads);

	//// 루트를 각 스레드의 작업 큐에 분배
	//size_t index = 0;
	//for (void* root : roots)
	//{
	//	markQueues[index % numThreads].push(root);
	//	++index;
	//}

	//// 각 스레드에서 마킹 작업 수행
	//std::vector<std::thread> threads; // <- 음 이거 매우 별론데... 매번 할당..?
	//for (size_t i = 0; i < numThreads; ++i)
	//{
	//	threads.emplace_back([this, i]() // <-이거 무슨 문법이지?
	//		{
	//			while (markQueues[i].empty() == true)
	//			{
	//				void* obj = markQueues[i].front();
	//				markQueues[i].pop();

	//				// 이미 마킹된 경우 건너뜀
	//				{
	//					std::lock_guard<std::mutex> lock(markedObjectsMutex);
	//					if (markedObjects.find(obj) != markedObjects.end())
	//					{
	//						continue;
	//					}
	//					markedObjects.insert(obj);
	//				}

	//				// 리플렉션 정보를 사용하여 참조된 객체 추가
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
			typeDesc->Delete(it->first); // 리플렉션 정보를 사용하여 객체 삭제
			it = heap.erase(it);         // 힙에서 제거
		}
		else 
		{
			++it;
		}
	}

	markedObjects.clear(); // 다음 GC를 위해 초기화
}

void GC::GarbageCollector::Collect(bool _isDump)
{
	/// [성능측정] std::cout << "\nStarting Garbage Collection...\n";
	Mark();
	//ConcurrentMark();
	Sweep(_isDump);
	/// [성능측정] std::cout << "Garbage Collection Complete.\n\n";
}
