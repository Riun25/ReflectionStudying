#pragma once
#include "Node.h"
#include "Timer.h"

class ObjectManager;
class TestFunctions
{
public:
	TestFunctions();
	~TestFunctions();

	void RunGCBasicTest(ObjectManager* _objectManager);
	void RunGCTest(ObjectManager* _objectManager, size_t _count);
	void RunGCTimerTest(ObjectManager* _objectManager, size_t _count);
	void RunGCRootsTest(ObjectManager* _objectManager, int _count = 5);

private:
	Timer* timer;
};