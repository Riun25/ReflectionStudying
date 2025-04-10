#pragma once
#include <random>
#include "Reflection/ReflectMacro.h"
#include "GarbageCollection/ObjectManager.h"
#define MAX_DEPTH 5

struct Node
{
	std::string key;
	int value;
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

std::vector<Node> CreateRandomRootGraph(ObjectManager* _manager, size_t _numRoots, size_t _numNodes);
void CreateRandomRootGraph(ObjectManager* _manager, size_t _numRoots, size_t _numNodes, std::unordered_set<void*>& _visited, std::vector<Node*>& _outRoots);

void TestRandomRootGraph(ObjectManager* _manager, size_t _numRoots, size_t _numNodes, std::unordered_set<void*>& _visited);

// �׽�Ʈ�� ���� �Ϻ� ��带 ��ŷ
void MarkRandomNodes(ObjectManager* _manager, size_t _numToMark);
void RegisterRecursively(Node* node, ObjectManager* manager, std::unordered_set<void*>& visited);

void PrintGraph(Node* _node, int _depth, int _maxDepth);