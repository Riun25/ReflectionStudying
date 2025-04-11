#pragma once
#include <random>
#include "Reflection/ReflectMacro.h"
#include "GarbageCollection/ObjectManager.h"
#include "ObjectInfo.h"
#define MAX_DEPTH 5

struct Node : public ObjectInfo
{
	std::string key;
	int value;
	std::vector<Node*> children;

	void Print() {}
	int Add(int a, int b) {	std::cout << a + b << "\n"; return a + b;}

	/*Node(std::string _key, int _value, std::vector<Node*> _children) :
		key(_key), value(_value), children(_children) {	}*/

	Node(std::string _key, int _value, std::vector<Node*> _children) :
		key(_key), value(_value), children(_children), ObjectInfo(false, Reflection.GetFullName()) {}

	REFLECT() // 이 유형에 대한 반사를 활성화
};

struct MyStruct
{
	void Print() {}

	REFLECT()
};

std::vector<Node*> CreateRandomRootGraph(ObjectManager* _manager, size_t _numRoots, size_t _numNodes, std::unordered_set<void*>& _visited, std::vector<Node*>& _allNodes);


// 테스트를 위한 일부 노드를 마킹
void MarkRandomNodes(ObjectManager* _manager, size_t _numToMark);

void PrintGraph(Node* _node, int _depth, int _maxDepth);