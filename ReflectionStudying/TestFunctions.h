#pragma once
#include "Node.h"
#include "Timer.h"

class TestFunctions
{
public:
	TestFunctions();
	~TestFunctions();

	void PrintClass();

	void RunGCBasicTest(ObjectManager* _objectManager);
	void RunGCTest(ObjectManager* _objectManager, size_t _count);
	void RunGCTimerTest(ObjectManager* _objectManager, size_t _count);
	/*void RunGCTimerTest_LimitPerFrame(ObjectManager* _objectManager, size_t _count);
	void RunPtrTests();
	void RunGCHeapTests();*/

private:
	void TestSharedPtr();
	void TestWeakPtr();
	void TestUniquePtr();
	void TestRawPtr();

private:
	//GC::GarbageCollector& gc;
	Timer* timer;
};