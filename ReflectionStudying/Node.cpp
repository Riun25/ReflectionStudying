#include "Node.h"

Node* CreateRandomGraph(GC::GarbageCollector& _gc, size_t _numNodes)
{
	std::vector<Node*> nodes;
	nodes.reserve(_numNodes);

	// 랜덤 노드 생성
	for (size_t i = 0; i < _numNodes; ++i)
	{
		Node* node = new Node{ "Node_" + std::to_string(i), static_cast<int>(i), {} };
		nodes.push_back(node);

		// GC에 객체와 리플렉션 정보 등록
		_gc.Allocate(static_cast<void*>(node), &Node::Reflection);
	}

	// 랜덤 연결 설정
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, _numNodes - 1);

	for (Node* node : nodes)
	{
		size_t numChildren = dist(gen) % 5; // 최대 5개의 자식 연결
		for (size_t j = 0; j < numChildren; ++j)
		{
			node->children.push_back(*nodes[dist(gen)]);
		}
	}

	return nodes[0]; // 루트 반환
}