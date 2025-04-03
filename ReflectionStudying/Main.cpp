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

	//1. ���� Ŭ������ �Լ� ����Ʈ �� ��� ���� ����Ʈ ����ϱ�
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// ���� �׷��� ����
	Node* node = new Node("Dog", 1, { {"Cat", 2, {}}, {"Rabbit", 3, {}} });
	typeDesc->Dump(node);
	delete node;

	TestFunctions* test = new TestFunctions(gc);
	// 2. GC�� �����Ǵ� ���Ƿ� ���� ����� 10000�� ������Ʈ ����� GC ���Ѽ� ���ŵǴ��� Ȯ���ϱ�
	test->RunGCTest(10000);

	// 3. GC ������ �����ϱ�
	for (int i = 0; i < 10; i++)
	{
		test->RunGCTimerTest(10000);
	}

	delete test;

	// 4. Ŭ���� ��ü�� �ּҿ� �Լ��� �̸��� �� ä��, ���� ���ڿ� ������ Ư�� Ŭ������ �Լ��� ȣ���ؼ� ȣ�� ��� ����ϱ�
	MultiParamClass obj(1, "TestObject");

	// ���� ȣ��
	RPCSystem::GetInstance().Invoke("MultiParamClass", "PrintInfo", &obj,
		{ std::string("Extra Information"), 42 });


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