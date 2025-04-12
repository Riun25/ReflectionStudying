#pragma once
#include <random>
#include "Reflection/ReflectMacro.h"
#include "ObjectInfo.h"

// ��ũ�� Ȱ��ȭ ����
#define USE_MULTITHREAD
#define THREAD_COUNT 8
#define MAX_DEPTH 5
//#define DEVIDE_PRINT

class ObjectManager;

struct Node : public ObjectInfo
{
	std::string key;
	int value;
	std::vector<Node*> children;

	void Print() {}
	int Add(int a, int b) {	std::cout << a + b << "\n"; return a + b;}

	Node(std::string _key, int _value, std::vector<Node*> _children) :
		key(_key), value(_value), children(_children), ObjectInfo(false, Reflection.GetFullName()) {}

	REFLECT() 
};

struct MyStruct
{
	void Print() {}

	REFLECT()
};

std::vector<Node*> CreateRandomRootGraph(ObjectManager* _manager, size_t _numRoots, size_t _numNodes, std::vector<Node*>& _allNodes);

// �׽�Ʈ�� ���� �Ϻ� ��带 ��ŷ
void MarkRandomNodes(ObjectManager* _manager, size_t _numToMark);

void PrintGraph(Node* _node, int _depth, int _maxDepth);