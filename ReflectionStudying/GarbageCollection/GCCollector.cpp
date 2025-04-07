#include "GCCollector.h"
//
//struct Node;
//
//void GC::GarbageCollector::Collect(bool _isDump)
//{
//	/*std::cout << "Registered Objects:\n";
//	for (const auto& [obj, typeDesc] : objectManager->GetAllObjects()) {
//		std::cout << "  Object: " << obj << ", Type: " << typeDesc->GetFullName() << "\n";
//	}*/
//
//	/// [성능측정] std::cout << "\nStarting Garbage Collection...\n";
//	Mark();
//	//ConcurrentMark();
//	Sweep(_isDump);
//	/// [성능측정] std::cout << "Garbage Collection Complete.\n\n";
//}
//
//void GC::GarbageCollector::Collect_LimitPerFrame(bool _isDump)
//{
//	Mark();
//	Sweep_LimitPerFrame(_isDump);
//}
//
//
//void GC::GarbageCollector::Mark()
//{
//	/// [성능측정] std::cout << "\nStarting Marking...\n";
//	if (objectManager->GetRootObjects().empty())
//	{
//		//std::cerr << "Warning: Root set is empty. No objects will be marked.\n";
//		return;
//	}
//
//
//	for (void* root : objectManager->GetRootObjects())
//	{
//		for (auto & e : markedObjects)
//		{
//			if (e == root)
//			{
//				continue;
//			}
//			markedObjects.emplace_back(root);
//			MarkObject(root);
//		}
//	}
//}
//
//void GC::GarbageCollector::MarkObject(void* _obj)
//{
//	// 이미 마킹된 경우 건너뜀
//	for (auto& e : markedObjects)
//	{
//		if (e == _obj)
//		{
//			continue;
//		}
//	}
//
//	markedObjects.emplace_back(_obj);
//	std::cout << "[Marking] Object: " << _obj << "\n";
//
//	// 참조된 객체 탐색
//	reflect::TypeDescriptor* baseTypeDesc;
//
//	auto it = objectManager->GetAllObjects().find(_obj);
//	if (it != objectManager->GetAllObjects().end())
//	{
//		baseTypeDesc = it->second;
//	}
//	else
//	{
//		std::cerr << "Error: Object not found in ObjectManager.\n";
//		return;
//	}
//
//	// TypeDescriptor_SubClass로 다운캐스팅
//	auto* typeDesc = dynamic_cast<reflect::TypeDescriptor_SubClass*>(baseTypeDesc);
//	if (!typeDesc)
//	{
//		std::cerr << "Error: TypeDescriptor is not a subclass type.\n";
//		return;
//	}
//
//	// 멤버 변수 추적 (리플렉션 정보 활용)
//	for (const auto& member : typeDesc->memberVec)
//	{
//		void* memberPtr = static_cast<char*>(_obj) + member.offset;
//
//		auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
//		if (vectorType)
//		{
//			size_t numItems = vectorType->GetSize(memberPtr);
//			for (size_t i = 0; i < numItems; ++i)
//			{
//				const void* item = vectorType->GetItem(memberPtr, i);
//
//				// Primitive 타입 무시
//				if (vectorType->itemType->IsPrimitive() == false)
//				{
//					MarkObject(const_cast<void*>(item));
//				}
//			}
//		}
//		else if (member.type->IsPrimitive() == false && objectManager->GetAllObjects().count(memberPtr))
//		{
//			MarkObject(memberPtr);
//		}
//	}
//}
//
//void GC::GarbageCollector::Sweep(bool _isDump)
//{
//	auto& allObjects = objectManager->GetAllObjects();
//
//	if (markedObjects.empty())
//	{
//		//std::cerr << "Warning: No objects were marked. All registered objects will be deleted.\n";
//		markedObjects.clear(); // 다음 GC를 위해 초기화
//		return;
//	}
//
//	// 1. 삭제할 객체 목록 수집
//	std::vector<void*> toDelete;
//	{
//		std::lock_guard<std::mutex> lock(sweepMutex);
//		for (auto it = allObjects.begin(); it != allObjects.end();)
//		{
//			if (markedObjects.find(it->first) == markedObjects.end())
//			{
//				toDelete.push_back(it->first);
//				++it;
//			}
//			else
//			{
//				++it;
//			}
//		}
//	}
//
//	// 2. 멀티스레드로 삭제 작업 병렬 처리
//	const size_t chunkSize = (toDelete.size() + numThreads - 1) / numThreads;
//	std::vector<std::thread> threads;
//
//	for (size_t t = 0; t < numThreads; ++t)
//	{
//		threads.emplace_back([&, t]()
//			{
//				const size_t start = t * chunkSize;
//				const size_t end = std::min(start + chunkSize, toDelete.size());
//
//				for (size_t i = start; i < end; ++i)
//				{
//					void* obj = toDelete[i];
//					if (_isDump)
//					{
//						std::lock_guard<std::mutex> lock(sweepMutex);
//						std::cout << "[Sweeping] Deleting Object: " << obj
//							<< ", Type: " << allObjects[obj]->GetFullName() << "\n";
//					}
//
//					// 자식 객체 재귀적 삭제
//					objectManager->DeleteChildren(obj, allObjects[obj]);
//
//					// 객체 메모리 해제
//					delete static_cast<Node*>(obj);
//				}
//			});
//	}
//
//	// 모든 스레드 작업 완료 대기
//	for (auto& thread : threads)
//	{
//		thread.join();
//	}
//
//	// 3. ObjectManager에서 삭제된 객체 제거
//	{
//		std::lock_guard<std::mutex> lock(sweepMutex);
//		for (void* obj : toDelete)
//		{
//			objectManager->DeregisterObject(obj);
//		}
//	}
//
//	markedObjects.clear(); // 다음 GC를 위해 초기화
//}
//
//void GC::GarbageCollector::Sweep_LimitPerFrame(bool _isDump)
//{
//	auto& allObjects = objectManager->GetAllObjects();
//	size_t deleteCount = 0;
//
//	for (auto it = allObjects.begin(); it != allObjects.end(); )
//	{
//		if (markedObjects.find(it->first) == markedObjects.end())
//		{
//			if (_isDump)
//			{
//				std::cout << "[Sweeping] Deleting Object: " << it->first
//					<< ", Type: " << it->second->GetFullName() << "\n";
//			}
//
//			objectManager->DeleteChildren_LimitPerFrame(it->first, it->second);
//			objectManager->DeregisterObject_LimitPerFrame(it->first); // 등록 해제
//			delete static_cast<Node*>(it->first);       // 메모리 해제
//			it = allObjects.erase(it);                  // map에서 제거
//
//			if (deleteCount >= maxDeletePerFrame)
//			{
//				break;
//			}
//		}
//		else
//		{
//			++it;
//		}
//	}
//	markedObjects.clear(); // 다음 GC를 위해 초기화
//}
