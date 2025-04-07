#include "Node.h"

Node* CreateRandomGraph(ObjectManager* _manager, size_t _numNodes)
{
	std::vector<Node*> nodes;
	nodes.reserve(_numNodes);

	// ��Ʈ ��� ����
	Node* root = new Node{ "Node_0", 0, {} };
	nodes.push_back(root);
	_manager->RegisterObject(static_cast<void*>(root), &Node::Reflection);

	// ���� ��� ���� �� ����
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1, _numNodes - 1);

	for (size_t i = 1; i < _numNodes; ++i) 
	{
		// ���ο� ��� ����
		Node* node = new Node{ "Node_" + std::to_string(i), static_cast<int>(i), {} };
		nodes.push_back(node);
		_manager->RegisterObject(static_cast<void*>(node), &Node::Reflection);

		// ���� �θ� ����
		int a = dist(gen) % nodes.size();
		Node* parent = nodes[a];
		parent->children.push_back(node); // �θ�-�ڽ� ���� ����
	}

	return root; // ��Ʈ ��ȯ
}

void MarkRandomNodes(ObjectManager* _manager, size_t _numToMark)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, _manager->GetObjects().size() - 1);

	for (size_t i = 1; i < _numToMark; ++i)
	{
		int index = dist(gen);
		_manager->GetObjects()[index].isMarked = true; // ������ ��ü�� ��ŷ
	}
}
