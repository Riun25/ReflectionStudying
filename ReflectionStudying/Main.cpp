#pragma once
#include "Timer.h"
#include "TestFunctions.h"
#include <vector>

struct Node;

class MultiParamClass
{
public:
	MultiParamClass(int id, std::string name) : id(id), name(std::move(name)) {}

	void PrintInfo(const std::string& extraInfo, int value)
	{
		std::cout << "Name: " << name << ", Extra: " << extraInfo << ", Value: " << value << std::endl;
	}

	REFLECT()

private:
	int id;
	std::string name;
};

int main()
{
	GC::GarbageCollector gc;

	TestFunctions* test = new TestFunctions(gc);
	for (int i = 0; i < 10; i++)
	{
		test->RunGCTimerTest(10000);
	}
	//test->RunGCHeapTests();
	//test->RunPtrTests();
	delete test;

	MultiParamClass obj(1, "TestObject");

	// 동적 호출
	RPCSystem::GetInstance().Invoke("MultiParamClass", "PrintInfo", &obj,
		{ std::string("Extra Information"), 42 });


	return 0;
}

// 노드 타입에 대한 정의를 나타내는 매크로
REFLECT_STRUCT_BEGIN(Node)
REFLECT_STRUCT_MEMBER(key)
REFLECT_STRUCT_MEMBER(value)
REFLECT_STRUCT_MEMBER(children)
REFLECT_STRUCT_FUNCTION(Node, Add)
REFLECT_STRUCT_END()

REFLECT_STRUCT_BEGIN(MultiParamClass)
REFLECT_STRUCT_FUNCTION(MultiParamClass, PrintInfo)
REFLECT_STRUCT_END()