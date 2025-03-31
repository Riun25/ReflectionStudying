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

	REFLECT() // 이 유형에 대한 반사를 활성화
};

struct MyStruct
{
	void Print() {}

	REFLECT()
};

Node* CreateRandomGraph(GC::GarbageCollector& _gc, int _numNodes)
{
	std::vector<Node*> nodes;

	// 랜덤 노드 생성
	for (int i = 0; i < _numNodes; ++i)
	{
		nodes.push_back(new Node{ "Node_" + std::to_string(i), i, {} });
		_gc.Allocate(nodes.back()); // 모든 노드를 GC에 등록
	}

	// 랜덤 연결 설정
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, _numNodes - 1);

	for (Node* node : nodes)
	{
		int numChildren = dist(gen) % 5; // 최대 5개의 자식 연결
		for (int j = 0; j < numChildren; ++j)
		{
			node->children.push_back(*nodes[dist(gen)]);
		}
	}

	return nodes[0]; // 루트 반환
}