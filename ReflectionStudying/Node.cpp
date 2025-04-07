#include "Node.h"

Node* CreateRandomGraph(ObjectManager* _manager, size_t _numNodes)
{
	std::vector<Node*> nodes;
	nodes.reserve(_numNodes);

	// 루트 노드 생성
	Node* root = new Node{ "Node_0", 0, {} };
	nodes.push_back(root);
	_manager->RegisterObject(static_cast<void*>(root), &Node::Reflection);

	// 랜덤 노드 생성 및 연결
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1, _numNodes - 1);

	for (size_t i = 1; i < _numNodes; ++i) 
	{
		// 새로운 노드 생성
		Node* node = new Node{ "Node_" + std::to_string(i), static_cast<int>(i), {} };
		nodes.push_back(node);
		_manager->RegisterObject(static_cast<void*>(node), &Node::Reflection);

		// 랜덤 부모 선택
		int a = dist(gen) % nodes.size();
		Node* parent = nodes[a];
		parent->children.push_back(node); // 부모-자식 관계 설정
	}

	return root; // 루트 반환
}

void MarkRandomNodes(ObjectManager* _manager, size_t _numToMark)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, _manager->GetObjects().size() - 1);

	for (size_t i = 1; i < _numToMark; ++i)
	{
		int index = dist(gen);
		_manager->GetObjects()[index].isMarked = true; // 임의의 객체를 마킹
	}
}
