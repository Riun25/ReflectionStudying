#pragma once
#include "ObjectManager.h"
#include "../Node.h"
#include "TypeDisposerRegistry.h"

class reflect::TypeDescriptor_StdVector;

void ObjectManager::RegisterObject(void* _obj, reflect::TypeDescriptor* _typeDesc)
{
	objects.emplace_back(_obj, _typeDesc);

	// Ÿ���� Ŭ������ ����ü���� Ȯ��
	auto* subClassType = dynamic_cast<reflect::TypeDescriptor_SubClass*>(_typeDesc);
	if (subClassType == nullptr) 
	{
		return; // Ŭ����/����ü�� �ƴ϶�� �ڽ� Ž�� ���ʿ�
	}

	// ��� ���� ��ȸ
	for (const auto& member : subClassType->memberVec) 
	{
		void* memberPtr = static_cast<char*>(_obj) + member.offset;

		// ���� Ÿ������ Ȯ��
		auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
		if (vectorType != nullptr) 
		{
			size_t numItems = vectorType->GetSize(memberPtr);
			for (size_t i = 0; i < numItems; ++i) 
			{
				void* child = const_cast<void*>(vectorType->GetItem(memberPtr, i));
				if (!vectorType->itemType->IsPrimitive()) 
				{ // ���� Ÿ���� �ƴ� ��쿡�� ���
					RegisterObject(child, vectorType->itemType); // ��������� ���
				}
			}
		}
		else if (!member.type->IsPrimitive()) 
		{
			// �Ϲ� ��� ������� ��������� ���
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
				if (vectorType != nullptr) // ���� Ÿ���̶�� ��������� ����
				{
					size_t numItems = vectorType->GetSize(memberPtr);
					for (size_t i = 0; i < numItems; ++i)
					{
						void* child = const_cast<void*>(vectorType->GetItem(memberPtr, i));
						if (vectorType->itemType->IsPrimitive() == false) // Primitive Ÿ�� ����
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

	// ��ŷ�� ��ü ����
	float markedRatio = static_cast<float>(markedCount) / totalObjects;

	// ó�� ��� ����
	if (markedRatio > 0.8) 
	{
		// ��κ� �����Ǵ� ���: ���� ���Ϳ��� ����
		SweepWithErase();
	}
	else if (markedRatio < 0.2) 
	{
		// ��κ� �����Ǵ� ���: ���ο� ���ͷ� ��ü
		SweepWithNewVector();
	}
	else 
	{
		// �߰� ����: ��Ƽ������� ó��
		SweepWithThreads();
	}
}

void ObjectManager::SweepWithErase()
{
	for (auto it = objects.begin(); it != objects.end();)
	{
		if (it->isMarked == false)
		{
			TypeDisposerRegistry::Dispose(it->type, it->object); // �޸� ����
			it = objects.erase(it);
		}
		else
		{
			it->isMarked = false; // ���� GC�� ���� �ʱ�ȭ
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
			TypeDisposerRegistry::Dispose(metaData.type, metaData.object); // �޸� ����
		}
		else
		{
			metaData.isMarked = false; // ���� GC�� ���� �ʱ�ȭ
			newObjects.push_back(metaData); // ������ ��ü�� ���ο� ���Ϳ� �߰�
		}
	}
	objects = std::move(newObjects); // ���� ���͸� ���ο� ���ͷ� ��ü
}

void ObjectManager::SweepWithThreads()
{
	int totalObjects = objects.size();
	int chunkSize = totalObjects / threadCount;

	std::vector<std::vector<ObjectMetaData>> threadResults(threadCount);
	std::vector<std::thread> threads;

	// �� ������ �۾� ����
	auto worker = [&](int threadIndex, int start, int end) 
		{
		std::vector<ObjectMetaData>& result = threadResults[threadIndex];
		for (int i = start; i < end; ++i) 
		{
			if (!objects[i].isMarked) 
			{
				TypeDisposerRegistry::Dispose(objects[i].type, objects[i].object); // �޸� ����
				objects[i].object = nullptr;
			}
			else 
			{
				objects[i].isMarked = false; // ��ŷ �ʱ�ȭ
				result.push_back(objects[i]); // ������ ��ü �߰�
			}
		}
		};

	// ������ ���� �� ������ ����
	for (int i = 0; i < threadCount; ++i) 
	{
		int start = i * chunkSize;
		int end = (i == threadCount - 1) ? totalObjects : start + chunkSize;
		threads.emplace_back(worker, i, start, end);
	}

	// ��� ������ �Ϸ� ���
	for (auto& t : threads) 
	{
		t.join();
	}

	// ��� ����
	std::vector<ObjectMetaData> newObjects;
	for (const auto& result : threadResults) 
	{
		newObjects.insert(newObjects.end(), result.begin(), result.end());
	}

	objects = std::move(newObjects); // ���� ���� ��ü
}
