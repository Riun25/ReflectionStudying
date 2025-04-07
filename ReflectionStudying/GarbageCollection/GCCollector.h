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
	//	// 가비지 컬렉션
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

	//	std::vector<void*> markedObjects; // 마킹된 객체 집합
	//private:
	//	ObjectManager* objectManager;
	//	size_t numThreads = 12;
	//	std::mutex sweepMutex; // 스레드 동기화용 뮤텍스
	//	size_t maxDeletePerFrame = 100;
	//};
}