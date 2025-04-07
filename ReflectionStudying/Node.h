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

	REFLECT() // 이 유형에 대한 반사를 활성화
};

struct MyStruct
{
	void Print() {}

	REFLECT()
};

Node* CreateRandomGraph(ObjectManager* _manager, size_t _numNodes);

// 테스트를 위한 일부 노드를 마킹
void MarkRandomNodes(ObjectManager* _manager, size_t _numToMark);