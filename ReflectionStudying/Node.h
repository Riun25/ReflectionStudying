#pragma once
#include <random>
#include "Reflection/Reflect.h"
#include "GarbageCollection/GCCollector.h"

struct Node
{
	std::string key;
	int value;
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

Node* CreateRandomGraph(GC::GarbageCollector& _gc, int _numNodes)
{
	std::vector<Node*> nodes;

	// ���� ��� ����
	for (int i = 0; i < _numNodes; ++i)
	{
		nodes.push_back(new Node{ "Node_" + std::to_string(i), i, {} });
		_gc.Allocate(nodes.back()); // ��� ��带 GC�� ���
	}

	// ���� ���� ����
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, _numNodes - 1);

	for (Node* node : nodes)
	{
		int numChildren = dist(gen) % 5; // �ִ� 5���� �ڽ� ����
		for (int j = 0; j < numChildren; ++j)
		{
			node->children.push_back(*nodes[dist(gen)]);
		}
	}

	return nodes[0]; // ��Ʈ ��ȯ
}