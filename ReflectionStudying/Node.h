#pragma once
#include <random>
#include "Reflection/ReflectMacro.h"
#include "GarbageCollection/ObjectManager.h"

struct Node
{
	std::string key;
	int value;
	//float floatValue;
	std::vector<Node*> children;

	void Print() {}
	int Add(int a, int b) {	std::cout << a + b << "\n"; return a + b;
	}

	REFLECT() // �� ������ ���� �ݻ縦 Ȱ��ȭ
};

struct MyStruct
{
	void Print() {}

	REFLECT()
};

Node* CreateRandomGraph(ObjectManager* _manager, size_t _numNodes);

// �׽�Ʈ�� ���� �Ϻ� ��带 ��ŷ
void MarkRandomNodes(ObjectManager* _manager, size_t _numToMark);