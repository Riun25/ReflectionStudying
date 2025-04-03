#pragma once
#include <random>
#include "Reflection/ReflectMacro.h"
#include "GarbageCollection/GCCollector.h"

struct Node
{
	std::string key;
	int value;
	//float floatValue;
	std::vector<Node> children;

	void Print() {}
	int Add(int a, int b) { return a + b; }

	REFLECT() // �� ������ ���� �ݻ縦 Ȱ��ȭ
};

struct MyStruct
{
	void Print() {}

	REFLECT()
};

Node* CreateRandomGraph(GC::GarbageCollector& _gc, size_t _numNodes);
