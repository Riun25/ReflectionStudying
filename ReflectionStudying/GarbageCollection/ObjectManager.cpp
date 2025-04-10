#pragma once
#include "ObjectManager.h"
#include "../Node.h"
#include "TypeDisposerRegistry.h"
#include <atomic>

class reflect::TypeDescriptor_StdVector;
class TypeDisposerRegistry;

void ObjectManager::RegisterObject(void* _obj, reflect::TypeDescriptor* _typeDesc, std::unordered_set<void*>& _visited)
{
	if (_visited.find(_obj) != _visited.end())
	{
		return; // 이미 등록된 객체라면 중단
	}

	_visited.insert(_obj); // 현재 객체를 방문 처리

	// 메타데이터 생성 및 등록
	objects.emplace_back(ObjectSlot{ _obj, _typeDesc });

	// 타입이 클래스나 구조체인지 확인
	auto* subClassType = dynamic_cast<reflect::TypeDescriptor_SubClass*>(_typeDesc);
	if (subClassType == nullptr)
	{
		return; // 클래스/구조체가 아니라면 자식 탐색 불필요
	}

	// 멤버 변수 순회
	for (const auto& member : subClassType->memberVec)
	{
		void* memberPtr = static_cast<char*>(_obj);

		// 벡터 타입인지 확인 -> 수동 처리
		auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
		if (vectorType != nullptr)
		{
			memberPtr = static_cast<char*>(_obj) + member.offset;


			//Node* node = static_cast<Node*>(_obj);
			//std::cout << "children field addr : " << &(node->children) << "\n";
			//std::cout << "offset-applied addr : " << static_cast<void*>(static_cast<char*>(_obj) + member.offset) << "\n";

			size_t numItems = vectorType->GetSize(memberPtr);
			//std::cout << numItems << "\n";
			for (size_t i = 0; i < numItems; ++i)
			{
				void* child = const_cast<void*>(vectorType->GetItem(memberPtr, i));
				if (!vectorType->itemType->IsPrimitive())
				{ // 원시 타입이 아닌 경우에만 등록
					RegisterObject(child, vectorType->itemType, _visited); // 재귀적으로 등록
				}
			}
		}
		else if (!member.type->IsPrimitive())
		{
			// 일반 멤버 변수라면 재귀적으로 등록
			RegisterObject(memberPtr, member.type, _visited);
		}
	}
}

void ObjectManager::MarkAll(const std::vector<void*>& roots)
{
	const int totalRoots = static_cast<int>(roots.size());
	const int chunkSize = (totalRoots + threadCount - 1) / threadCount; // ceil
	std::vector<std::thread> threads;

	for (int i = 0; i < threadCount; ++i)
	{
		int start = i * chunkSize;
		int end = std::min(start + chunkSize, totalRoots);

		threads.emplace_back([this, &roots, start, end]()
			{
				std::vector<void*> queue;
				size_t cursor = 0;

				// 각 루트를 큐에 넣기
				for (int r = start; r < end; ++r)
					queue.push_back(roots[r]);

				while (cursor < queue.size())
				{
					void* current = queue[cursor++];

					auto it = objectIndex.find(current);
					if (it == objectIndex.end()) continue;

					ObjectSlot* metaData = it->second;
					if (!metaData->TryMark()) continue;

					auto* typeDesc = dynamic_cast<reflect::TypeDescriptor_SubClass*>(metaData->type);
					if (!typeDesc) continue;

					for (const auto& member : typeDesc->memberVec)
					{
						auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
						if (!vectorType) continue;

						void* memberPtr = static_cast<char*>(current) + member.offset;
						size_t numItems = vectorType->GetSize(memberPtr);

						for (size_t i = 0; i < numItems; ++i)
						{
							void* child = const_cast<void*>(vectorType->GetItem(memberPtr, i));
							if (!vectorType->itemType->IsPrimitive())
								queue.push_back(child);
						}
					}
				}
			});
	}

	for (auto& t : threads)
		t.join();
}

void ObjectManager::Mark(void* _root)
{
	std::vector<void*> queue;
	size_t cursor = 0;
	queue.push_back(_root);

	while (cursor < queue.size())
	{
		void* current = queue[cursor++];

		auto it = objectIndex.find(current);
		if (it == objectIndex.end())
		{
			continue;
		}

		ObjectSlot* metaData = it->second; // 직접 참조로 가져오기
		if (!metaData->TryMark()) continue;

		auto* typeDesc = dynamic_cast<reflect::TypeDescriptor_SubClass*>(metaData->type);
		if (typeDesc == nullptr)
		{
			continue;
		}

		for (const auto& member : typeDesc->memberVec)
		{
			auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
			if (vectorType != nullptr) // 벡터 타입이라면 재귀적으로 진행
			{
				void* memberPtr = static_cast<char*>(current) + member.offset;

				// 디버깅용
				//Node* node = static_cast<Node*>(_root);
				//std::cout << "children field addr : " << &(node->children) << "\n";
				//std::cout << "offset-applied addr : " << static_cast<void*>(static_cast<char*>(_root) + member.offset) << "\n";

				// 벡터 데이터 접근
				size_t numItems = vectorType->GetSize(memberPtr);
				for (size_t i = 0; i < numItems; ++i)
				{
					void* child = const_cast<void*>(vectorType->GetItem(memberPtr, i));
					//std::cout << "void* child : " << child << "\n";
					if (vectorType->itemType->IsPrimitive() == false) // Primitive 타입 무시
					{
						queue.push_back(child);
					}
				}
			}
		}
	}
}

void ObjectManager::MarkTest(int _numRoot)
{
	// 마킹 초기화
	for (auto* slot : objects2)
		slot->ClearMark();

	const int chunkSize = (_numRoot + threadCount - 1) / threadCount;
	std::vector<std::thread> threads;

	for (int t = 0; t < threadCount; ++t)
	{
		int start = t * chunkSize;
		int end = std::min(start + chunkSize, _numRoot);

		threads.emplace_back([this, start, end]() 
			{
			std::vector<ObjectSlot*> queue;
			size_t cursor = 0;

			// 루트 슬롯 큐에 추가
			for (int i = start; i < end; ++i)
				queue.push_back(objects2[i]);

			while (cursor < queue.size())
			{
				ObjectSlot* slot = queue[cursor++];
				if (!slot->TryMark()) continue;

				auto* typeDesc = dynamic_cast<reflect::TypeDescriptor_SubClass*>(slot->type);
				if (!typeDesc) continue;

				for (const auto& member : typeDesc->memberVec)
				{
					auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
					if (!vectorType) continue;

					void* memberPtr = static_cast<char*>(slot->object) + member.offset;
					size_t numItems = vectorType->GetSize(memberPtr);

					for (size_t i = 0; i < numItems; ++i)
					{
						void* childPtr = const_cast<void*>(vectorType->GetItem(memberPtr, i));
						if (vectorType->itemType->IsPrimitive()) continue;

						// 포인터 매칭 기반 - 객체 전체에서 직접 ObjectSlot* 찾기
						for (auto* candidate : objects2)
						{
							if (candidate->object == childPtr)
							{
								queue.push_back(candidate);
								break;
							}
						}
					}
				}
			}
			});
	}

	for (auto& t : threads)
		t.join();
}


void ObjectManager::Sweep()
{
	std::partition(objects.begin(), objects.end(), [](const auto& obj) { return obj.IsMarked(); });

	SweepWithThreads();
}

void ObjectManager::PrintObject()
{
	for (size_t i = 0; i < objects.size(); ++i)
	{
		std::cout << "Object [" << i << "] " << objects[i].object << "\n";
	}
}

void ObjectManager::CountMarkObject()
{
	markedCount = std::count_if(objects.begin(), objects.end(), [](const auto& obj)
		{return obj.IsMarked(); });

	std::cout << markedCount << "\t";
}

void ObjectManager::FilledObjIdx()
{
	for (auto& obj : objects)
	{
		//std::cout << "[Register] _obj = " << obj << "\n";
		//std::cout << "[Register] &objects.back() = " << &(objects.back()) << "\n";
		objectIndex[obj.object] = &obj;
	}
}

void ObjectManager::SweepWithErase()
{
	//for (auto it = objects.begin(); it != objects.end();)
	//{
	//	if (!it->isMarked)
	//	{
	//		TypeDisposerRegistry::Dispose(it->type, it->object);
	//		objectIndex.erase(it->object); // 인덱스 제거
	//		it = objects.erase(it);
	//	}
	//	else
	//	{
	//		it->isMarked = false;
	//		++it;
	//	}
	//}
}

void ObjectManager::SweepWithNewVector(int _markedCount)
{
	//std::vector<ObjectMetaData> newObjects;
	//newObjects.reserve(_markedCount); // 성능 최적화

	//for (auto& metaData : objects)
	//{
	//	if (!metaData.isMarked)
	//	{
	//		TypeDisposerRegistry::Dispose(metaData.type, metaData.object);
	//		objectIndex.erase(metaData.object); // 인덱스 제거
	//	}
	//	else
	//	{
	//		metaData.isMarked = false;
	//		newObjects.emplace_back(std::move(metaData)); // move로 최적화
	//	}
	//}

	//objects = std::move(newObjects); // 교체
}


void ObjectManager::SweepWithThreads()
{
	int totalObjects = objects.size();
	int chunkSize = totalObjects / threadCount;

	std::vector<std::vector<ObjectSlot>> threadResults(threadCount);
	std::vector<std::thread> threads;

	// 사전에 슬라이스 영역 배정
	std::vector<ObjectSlot> newObjects(totalObjects); // 예약 + 공간 확보

	auto worker = [&](int tid, int start, int end)
		{
			auto& local = threadResults[tid];
			for (int i = start; i < end; ++i)
			{
				if (!objects[i].IsMarked())
				{
					TypeDisposerRegistry::Dispose(objects[i].type, objects[i].object);
				}
				else 
				{
					objects[i].ClearMark();
					local.push_back(std::move(objects[i]));
				}
			}
		};

	// 스레드 생성 및 데이터 분할
	for (int i = 0; i < threadCount; ++i)
	{
		int start = i * chunkSize;
		int end = (i == threadCount - 1) ? totalObjects : start + chunkSize;
		threads.emplace_back(worker, i, start, end);
	}

	// 모든 스레드 완료 대기
	for (auto& t : threads)
	{
		t.join();
	}

	// 병합: 살아남은 객체만 합치기
	for (auto& localVec : threadResults)
	{
		std::move(localVec.begin(), localVec.end(), std::back_inserter(newObjects));
	}

	newObjects.resize(markedCount);
	objects = std::move(newObjects);
}

