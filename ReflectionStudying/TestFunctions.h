#pragma once
#include "Node.h"
#include "Timer.h"

class TestFunctions
{
public:
	TestFunctions(GC::GarbageCollector& _gc);
	~TestFunctions();

	void RunGCTimerTest(size_t _count);
	void RunPtrTests();
	void RunGCHeapTests();

private:
	void TestSharedPtr();
	void TestWeakPtr();
	void TestUniquePtr();
	void TestRawPtr();

private:
	GC::GarbageCollector& gc;
	Timer* timer;
};