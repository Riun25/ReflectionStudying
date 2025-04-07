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
//	/// [��������] std::cout << "\nStarting Garbage Collection...\n";
//	Mark();
//	//ConcurrentMark();
//	Sweep(_isDump);
//	/// [��������] std::cout << "Garbage Collection Complete.\n\n";
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
//	/// [��������] std::cout << "\nStarting Marking...\n";
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
//	// �̹� ��ŷ�� ��� �ǳʶ�
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
//	// ������ ��ü Ž��
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
//	// TypeDescriptor_SubClass�� �ٿ�ĳ����
//	auto* typeDesc = dynamic_cast<reflect::TypeDescriptor_SubClass*>(baseTypeDesc);
//	if (!typeDesc)
//	{
//		std::cerr << "Error: TypeDescriptor is not a subclass type.\n";
//		return;
//	}
//
//	// ��� ���� ���� (���÷��� ���� Ȱ��)
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
//				// Primitive Ÿ�� ����
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
//		markedObjects.clear(); // ���� GC�� ���� �ʱ�ȭ
//		return;
//	}
//
//	// 1. ������ ��ü ��� ����
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
//	// 2. ��Ƽ������� ���� �۾� ���� ó��
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
//					// �ڽ� ��ü ����� ����
//					objectManager->DeleteChildren(obj, allObjects[obj]);
//
//					// ��ü �޸� ����
//					delete static_cast<Node*>(obj);
//				}
//			});
//	}
//
//	// ��� ������ �۾� �Ϸ� ���
//	for (auto& thread : threads)
//	{
//		thread.join();
//	}
//
//	// 3. ObjectManager���� ������ ��ü ����
//	{
//		std::lock_guard<std::mutex> lock(sweepMutex);
//		for (void* obj : toDelete)
//		{
//			objectManager->DeregisterObject(obj);
//		}
//	}
//
//	markedObjects.clear(); // ���� GC�� ���� �ʱ�ȭ
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
//			objectManager->DeregisterObject_LimitPerFrame(it->first); // ��� ����
//			delete static_cast<Node*>(it->first);       // �޸� ����
//			it = allObjects.erase(it);                  // map���� ����
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
//	markedObjects.clear(); // ���� GC�� ���� �ʱ�ȭ
//}
