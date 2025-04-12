#pragma once
#include "Timer.h"
#include "TestFunctions.h"
#include "GarbageCollection/ObjectManager.h"
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
#ifdef USE_MULTITHREAD
	std::cout << "Thread Num: " << THREAD_COUNT;
	std::cout << " Using Multi-Thread\n";
#else
	std::cout << " Using Single-Thread\n";
#endif
	ObjectManager* objectManager = new ObjectManager();

	//1. 임의 클래스의 함수 리스트 및 멤버 변수 리스트 출력하기
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	//// 랜덤 그래프 생성
	Node* node = new Node("Dog", 1, { new Node{"Cat", 2, {}}, new Node{"Rabbit", 3, {}} });
	//typeDesc->Dump(node);
	delete node;

	TestFunctions* test = new TestFunctions();
	// 2. GC가 제대로 동작하는지 확인하기
	//test->RunGCBasicTest(objectManager);

	// 3. GC 성능을 측정하기 : GC로 관리되는 임의로 서로 연결된 10000개 오브젝트 만들고 GC 시켜서 제거되는지 확인하기
	for (int i = 0; i < 30; i++)
	{
		//test->RunGCTimerTest(objectManager, 100000);
	}

	std::cout << "=============================\n";

	/* 4. 클래스 개체의 주소와 함수의 이름을 모른 채로, 오직 문자열 만으로 특정 클래스의 함수를 호출해서 호출 결과 출력하기
	//MultiParamClass obj(1, "TestObject");

	// 동적 호출
	//RPCSystem::GetInstance().Invoke("MultiParamClass", "PrintInfo", &obj,
	//	{ std::string("Extra Information"), 42 });*/

	//RPCSystem::GetInstance().Invoke("Node", "Add", &node,
	//	{ 42, 24 });

	for (int i = 1; i < 11; ++i)
	{
		std::cout << "==============[ " << i << " ]=============== \n";

		//test->RunGCRootsTest(objectManager, numRoots, numNodes);
		test->RunGCRootsTest(objectManager);

	}

	delete test;
	delete objectManager;

	std::cout << "Press Enter to exit...\n";
	std::cin.get();  // 콘솔 대기

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