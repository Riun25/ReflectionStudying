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

	//1. ���� Ŭ������ �Լ� ����Ʈ �� ��� ���� ����Ʈ ����ϱ�
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	//// ���� �׷��� ����
	Node* node = new Node("Dog", 1, { new Node{"Cat", 2, {}}, new Node{"Rabbit", 3, {}} });
	//typeDesc->Dump(node);
	delete node;

	TestFunctions* test = new TestFunctions();
	// 2. GC�� ����� �����ϴ��� Ȯ���ϱ�
	//test->RunGCBasicTest(objectManager);

	// 3. GC ������ �����ϱ� : GC�� �����Ǵ� ���Ƿ� ���� ����� 10000�� ������Ʈ ����� GC ���Ѽ� ���ŵǴ��� Ȯ���ϱ�
	for (int i = 0; i < 30; i++)
	{
		//test->RunGCTimerTest(objectManager, 100000);
	}

	std::cout << "=============================\n";

	/* 4. Ŭ���� ��ü�� �ּҿ� �Լ��� �̸��� �� ä��, ���� ���ڿ� ������ Ư�� Ŭ������ �Լ��� ȣ���ؼ� ȣ�� ��� ����ϱ�
	//MultiParamClass obj(1, "TestObject");

	// ���� ȣ��
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
	std::cin.get();  // �ܼ� ���

	return 0;
}

// ��� Ÿ�Կ� ���� ���Ǹ� ��Ÿ���� ��ũ��
REFLECT_STRUCT_BEGIN(Node)
REFLECT_STRUCT_MEMBER(key)
REFLECT_STRUCT_MEMBER(value)
REFLECT_STRUCT_MEMBER(children)
REFLECT_STRUCT_FUNCTION(Node, Add)
REFLECT_STRUCT_END()

REFLECT_STRUCT_BEGIN(MultiParamClass)
REFLECT_STRUCT_FUNCTION(MultiParamClass, PrintInfo)
REFLECT_STRUCT_END()