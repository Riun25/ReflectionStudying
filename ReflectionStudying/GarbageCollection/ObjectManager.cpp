#pragma once
#include "ObjectManager.h"
#include "../Node.h"
#include "TypeDisposerRegistry.h"

class reflect::TypeDescriptor_StdVector;

void ObjectManager::RegisterObject(void* _obj, reflect::TypeDescriptor* _typeDesc)
{
	objects.emplace_back(_obj, _typeDesc);

	// 타입이 클래스나 구조체인지 확인
	auto* subClassType = dynamic_cast<reflect::TypeDescriptor_SubClass*>(_typeDesc);
	if (subClassType == nullptr) 
	{
		return; // 클래스/구조체가 아니라면 자식 탐색 불필요
	}

	// 멤버 변수 순회
	for (const auto& member : subClassType->memberVec) 
	{
		void* memberPtr = static_cast<char*>(_obj) + member.offset;

		// 벡터 타입인지 확인
		auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
		if (vectorType != nullptr) 
		{
			size_t numItems = vectorType->GetSize(memberPtr);
			for (size_t i = 0; i < numItems; ++i) 
			{
				void* child = const_cast<void*>(vectorType->GetItem(memberPtr, i));
				if (!vectorType->itemType->IsPrimitive()) 
				{ // 원시 타입이 아닌 경우에만 등록
					RegisterObject(child, vectorType->itemType); // 재귀적으로 등록
				}
			}
		}
		else if (!member.type->IsPrimitive()) 
		{
			// 일반 멤버 변수라면 재귀적으로 등록
			RegisterObject(memberPtr, member.type);
		}
	}
}

void ObjectManager::Mark(void* _root)
{
	for (auto& metaData : objects)
	{
		if (metaData.object == _root && metaData.isMarked == false)
		{
			metaData.isMarked = true;

			auto* typeDesc = dynamic_cast<reflect::TypeDescriptor_SubClass*>(metaData.type);
			if (typeDesc == nullptr)
			{
				return;
			}

			for (const auto& member : typeDesc->memberVec)
			{
				void* memberPtr = static_cast<char*>(_root) + member.offset;

				auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
				if (vectorType != nullptr) // 벡터 타입이라면 재귀적으로 진행
				{
					size_t numItems = vectorType->GetSize(memberPtr);
					for (size_t i = 0; i < numItems; ++i)
					{
						void* child = const_cast<void*>(vectorType->GetItem(memberPtr, i));
						if (vectorType->itemType->IsPrimitive() == false) // Primitive 타입 무시
						{
							Mark(child);
						}
					}
				}
			}
		}
	}
}

void ObjectManager::Sweep()
{
	int totalObjects = objects.size();
	int markedCount = 0;

	for (const auto& metaData : objects)
	{
		if (metaData.isMarked == true)
		{
			++markedCount;
		}
	}

	// 마킹된 객체 비율
	float markedRatio = static_cast<float>(markedCount) / totalObjects;

	// 처리 방식 결정
	if (markedRatio > 0.8) 
	{
		// 대부분 유지되는 경우: 기존 벡터에서 삭제
		SweepWithErase();
	}
	else if (markedRatio < 0.2) 
	{
		// 대부분 삭제되는 경우: 새로운 벡터로 교체
		SweepWithNewVector();
	}
	else 
	{
		// 중간 비율: 멀티스레드로 처리
		SweepWithThreads();
	}
}

void ObjectManager::SweepWithErase()
{
	for (auto it = objects.begin(); it != objects.end();)
	{
		if (it->isMarked == false)
		{
			TypeDisposerRegistry::Dispose(it->type, it->object); // 메모리 해제
			it = objects.erase(it);
		}
		else
		{
			it->isMarked = false; // 다음 GC를 위해 초기화
			++it;
		}
	}
}

void ObjectManager::SweepWithNewVector()
{
	std::vector<ObjectMetaData> newObjects;

	for (auto& metaData : objects)
	{
		if (metaData.isMarked == false)
		{
			TypeDisposerRegistry::Dispose(metaData.type, metaData.object); // 메모리 해제
		}
		else
		{
			metaData.isMarked = false; // 다음 GC를 위해 초기화
			newObjects.push_back(metaData); // 유지할 객체만 새로운 벡터에 추가
		}
	}
	objects = std::move(newObjects); // 기존 벡터를 새로운 벡터로 교체
}

void ObjectManager::SweepWithThreads()
{
	int totalObjects = objects.size();
	int chunkSize = totalObjects / threadCount;

	std::vector<std::vector<ObjectMetaData>> threadResults(threadCount);
	std::vector<std::thread> threads;

	// 각 스레드 작업 정의
	auto worker = [&](int threadIndex, int start, int end) 
		{
		std::vector<ObjectMetaData>& result = threadResults[threadIndex];
		for (int i = start; i < end; ++i) 
		{
			if (!objects[i].isMarked) 
			{
				TypeDisposerRegistry::Dispose(objects[i].type, objects[i].object); // 메모리 해제
				objects[i].object = nullptr;
			}
			else 
			{
				objects[i].isMarked = false; // 마킹 초기화
				result.push_back(objects[i]); // 유지할 객체 추가
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

	// 결과 병합
	std::vector<ObjectMetaData> newObjects;
	for (const auto& result : threadResults) 
	{
		newObjects.insert(newObjects.end(), result.begin(), result.end());
	}

	objects = std::move(newObjects); // 기존 벡터 교체
}
