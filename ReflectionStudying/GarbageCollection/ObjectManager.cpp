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
		return; // �̹� ��ϵ� ��ü��� �ߴ�
	}

	_visited.insert(_obj); // ���� ��ü�� �湮 ó��

	// ��Ÿ������ ���� �� ���
	objects.emplace_back(ObjectSlot{ _obj, _typeDesc });

	// Ÿ���� Ŭ������ ����ü���� Ȯ��
	auto* subClassType = dynamic_cast<reflect::TypeDescriptor_SubClass*>(_typeDesc);
	if (subClassType == nullptr)
	{
		return; // Ŭ����/����ü�� �ƴ϶�� �ڽ� Ž�� ���ʿ�
	}

	// ��� ���� ��ȸ
	for (const auto& member : subClassType->memberVec)
	{
		void* memberPtr = static_cast<char*>(_obj);

		// ���� Ÿ������ Ȯ�� -> ���� ó��
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
				{ // ���� Ÿ���� �ƴ� ��쿡�� ���
					RegisterObject(child, vectorType->itemType, _visited); // ��������� ���
				}
			}
		}
		else if (!member.type->IsPrimitive())
		{
			// �Ϲ� ��� ������� ��������� ���
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

				// �� ��Ʈ�� ť�� �ֱ�
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

		ObjectSlot* metaData = it->second; // ���� ������ ��������
		if (!metaData->TryMark()) continue;

		auto* typeDesc = dynamic_cast<reflect::TypeDescriptor_SubClass*>(metaData->type);
		if (typeDesc == nullptr)
		{
			continue;
		}

		for (const auto& member : typeDesc->memberVec)
		{
			auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
			if (vectorType != nullptr) // ���� Ÿ���̶�� ��������� ����
			{
				void* memberPtr = static_cast<char*>(current) + member.offset;

				// ������
				//Node* node = static_cast<Node*>(_root);
				//std::cout << "children field addr : " << &(node->children) << "\n";
				//std::cout << "offset-applied addr : " << static_cast<void*>(static_cast<char*>(_root) + member.offset) << "\n";

				// ���� ������ ����
				size_t numItems = vectorType->GetSize(memberPtr);
				for (size_t i = 0; i < numItems; ++i)
				{
					void* child = const_cast<void*>(vectorType->GetItem(memberPtr, i));
					//std::cout << "void* child : " << child << "\n";
					if (vectorType->itemType->IsPrimitive() == false) // Primitive Ÿ�� ����
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
	// ��ŷ �ʱ�ȭ
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

			// ��Ʈ ���� ť�� �߰�
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

						// ������ ��Ī ��� - ��ü ��ü���� ���� ObjectSlot* ã��
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
	//		objectIndex.erase(it->object); // �ε��� ����
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
	//newObjects.reserve(_markedCount); // ���� ����ȭ

	//for (auto& metaData : objects)
	//{
	//	if (!metaData.isMarked)
	//	{
	//		TypeDisposerRegistry::Dispose(metaData.type, metaData.object);
	//		objectIndex.erase(metaData.object); // �ε��� ����
	//	}
	//	else
	//	{
	//		metaData.isMarked = false;
	//		newObjects.emplace_back(std::move(metaData)); // move�� ����ȭ
	//	}
	//}

	//objects = std::move(newObjects); // ��ü
}


void ObjectManager::SweepWithThreads()
{
	int totalObjects = objects.size();
	int chunkSize = totalObjects / threadCount;

	std::vector<std::vector<ObjectSlot>> threadResults(threadCount);
	std::vector<std::thread> threads;

	// ������ �����̽� ���� ����
	std::vector<ObjectSlot> newObjects(totalObjects); // ���� + ���� Ȯ��

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

	// ����: ��Ƴ��� ��ü�� ��ġ��
	for (auto& localVec : threadResults)
	{
		std::move(localVec.begin(), localVec.end(), std::back_inserter(newObjects));
	}

	newObjects.resize(markedCount);
	objects = std::move(newObjects);
}

