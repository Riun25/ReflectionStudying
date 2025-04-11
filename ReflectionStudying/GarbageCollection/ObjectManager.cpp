#pragma once
#include "ObjectManager.h"
#include "../Node.h"
#include <atomic>

class reflect::TypeDescriptor_StdVector;
class TypeDisposerRegistry;

void ObjectManager::Mark(const std::string& _typeName, ObjectInfo* _obj)
{
	if (_obj->isMarked == true) return; // �̹� ��ŷ��

	_obj->isMarked = true; // ��ŷ ����
	++markedCount;

	auto* typeDesc = FindTypeDescriptor(_obj->TypeName);
		//dynamic_cast<reflect::TypeDescriptor_SubClass*>(it->second.second);
	if (typeDesc == nullptr){return;}

	for (const auto& member : typeDesc->memberVec)
	{
		auto* vectorType = dynamic_cast<reflect::TypeDescriptor_StdVector*>(member.type);
		if (vectorType != nullptr) // ���� Ÿ���̶�� ��������� ����
		{
			void* memberPtr = static_cast<char*>(static_cast<void*>(_obj)) + member.offset;
			// ������
			//Node* node = static_cast<Node*>(_root);
			//std::cout << "children field addr : " << &(node->children) << "\n";
			//std::cout << "offset-applied addr : " << static_cast<void*>(static_cast<char*>(_root) + member.offset) << "\n";
			
			// ���� ������ ����
			size_t numItems = vectorType->GetSize(memberPtr);
			for (size_t i = 0; i < numItems; ++i)
			{
				ObjectInfo* child = static_cast<ObjectInfo*>(const_cast<void*>(vectorType->GetItem(memberPtr, i)));
				//std::cout << "void* child : " << child << "\n";
				if (vectorType->itemType->IsPrimitive() == false) // Primitive Ÿ�� ����
				{
					Mark(_obj->TypeName, child);
				}
			}
		}
	}
}

void ObjectManager::Sweep(std::vector<ObjectInfo*>* _nodes)
{
	std::vector<ObjectInfo*>& nodes = *_nodes;

	nodes.erase(std::remove_if(nodes.begin(), nodes.end(),
		[](ObjectInfo* obj) {if (!obj->isMarked) { delete obj; return true; }return false; }),
		nodes.end());
}

void ObjectManager::MarkwithThread(const std::string& _typeName, std::vector<ObjectInfo*>* _nodes, int _count)
{
	std::vector<ObjectInfo*>& allNodes = *_nodes;
	int chunkSize = (_count + THREAD_COUNT - 1) / THREAD_COUNT;

	std::thread threads[THREAD_COUNT];

	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		int start = i * chunkSize;
		int end = std::min(start + chunkSize, _count);

		threads[i]= std::thread([this, &allNodes, start, end, _typeName]()
			{
				for (int j = start; j < end; ++j)
				{
					this->Mark(_typeName, allNodes[j]); // ���� ��� ��ũ �Լ� ȣ��
				}
			});
	}

	for (auto& t : threads)
		t.join();
}


void ObjectManager::SweepwithThread(std::vector<ObjectInfo*>* _nodes)
{
	std::vector<ObjectInfo*>& nodes = *_nodes;
	int objectSize = static_cast<int>(nodes.size());
	int chunkSize = (objectSize + THREAD_COUNT - 1) / THREAD_COUNT;

	std::thread threads[THREAD_COUNT];
	std::vector<ObjectInfo*> threadSurvivors[THREAD_COUNT]; // ������ �迭

	// �̸� ũ�� ����
	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		threadSurvivors[i].reserve(chunkSize);
	}

	for (int t = 0; t < THREAD_COUNT; ++t)
	{
		int start = t * chunkSize;
		int end = std::min(start + chunkSize, objectSize);

		threads[t] = std::thread([this, &nodes, &threadSurvivors, start, end, t]()
			{
				for (int i = start; i < end; ++i)
				{
					ObjectInfo* current = nodes[i];

					if (current->isMarked)
					{
						threadSurvivors[t].push_back(current);
					}
					else
					{
						delete current;
						current = nullptr;
					}
				}
			});
	}

	for (auto& thread : threads)
		thread.join();

	// ����
	std::vector<ObjectInfo*> finalSurvivors;
	finalSurvivors.reserve(markedCount);

	int offset = 0;
	for (int t = 0; t < THREAD_COUNT; ++t)
	{
		auto& local = threadSurvivors[t];
		std::move(local.begin(), local.end(), finalSurvivors.begin() + offset);
		offset += static_cast<int>(local.size());
	}

	nodes.reserve(markedCount);
	nodes = std::move(finalSurvivors);
}


void ObjectManager::CountMarkObject()
{
	std::cout << markedCount << "\t";
}

void ObjectManager::RegisterTypeDiscriptor(const std::string& _typeName, reflect::TypeDescriptor_SubClass* _s)
{
	for (auto& e : testVec)
	{
		if (_typeName == e.first)
		{
			return;
		}
	}
	testVec.push_back({ _typeName, _s });
}

reflect::TypeDescriptor_SubClass* ObjectManager::FindTypeDescriptor(const std::string& _typeName)
{
	for (auto& e : testVec)
	{
		if (_typeName == e.first)
		{
			return e.second;
		}
	}
	return nullptr;
}