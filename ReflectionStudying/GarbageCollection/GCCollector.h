#pragma once
#include <vector>
#include <queue>

#include <atomic>
#include "ObjectManager.h"

class ObjectManager;
class reflect::TypeDescriptor;

namespace GC
{
	//class GarbageCollector
	//{
	//public:
	//	GarbageCollector(ObjectManager* _manager) : objectManager(_manager) {}
	//	~GarbageCollector() {}

	//public:
	//	// ������ �÷���
	//	void Collect(bool _isDump = false);
	//	void Collect_LimitPerFrame(bool _isDump = false);
	//	void SetNumThreads(size_t _threads)
	//	{
	//		numThreads = _threads;
	//	}

	//private:
	//	void Mark();
	//	void MarkObject(void* _obj);
	//	void Sweep(bool _isDump);
	//	void Sweep_LimitPerFrame(bool _isDump);

	//	std::vector<void*> markedObjects; // ��ŷ�� ��ü ����
	//private:
	//	ObjectManager* objectManager;
	//	size_t numThreads = 12;
	//	std::mutex sweepMutex; // ������ ����ȭ�� ���ؽ�
	//	size_t maxDeletePerFrame = 100;
	//};
}