#include "Node.h"

std::vector<Node*> CreateRandomRootGraph(ObjectManager* _manager, size_t _numRoots, size_t _numNodes, std::unordered_set<void*>& _visited, std::vector<Node*>& _allNodes)
{
	std::vector<Node*> allNodes = _allNodes;
	allNodes.reserve(_numNodes); // 선택적 최적화

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, _numNodes - 1);

	// 각 노드의 깊이를 저장하는 맵 (노드 포인터 -> 깊이)
	std::unordered_map<Node*, size_t> nodeDepth;

	// 랜덤 루트 노드 생성 및 연결
	for (size_t i = 0; i < _numRoots; ++i)
	{
		// 새로운 노드 생성
		Node* root = new Node{ "Root_" + std::to_string(i), static_cast<int>(i), {} };
		nodeDepth[root] = 0;
		//_manager->RegisterObject(static_cast<void*>(root), &Node::Reflection, _visited);
		allNodes.push_back(root); // 랜덤 접근용
		_manager->RegisterTypeDiscriptor(root->TypeName, &root->Reflection);
	}

	// 자식 노드 생성 및 연결
	for (size_t i = _numRoots; i < _numNodes; ++i)
	{
		// 새로운 노드 생성
		Node* pNode = new Node{ "Child_" + std::to_string(i), static_cast<int>(i), {} };
		//_manager->RegisterObject(static_cast<void*>(pNode), &Node::Reflection, _visited);

		constexpr int maxTries = 5;
		bool connected = false;

		for (int tries = 0; tries < maxTries; ++tries)
		{
			Node* parent = allNodes[dist(gen) % i]; // O(1) 랜덤 접근

			if (parent != pNode && nodeDepth[parent] < MAX_DEPTH)
			{
				parent->children.push_back(pNode);
				nodeDepth[pNode] = nodeDepth[parent] + 1;
				connected = true;
				break;
			}
		}

		if (!connected)
		{
			Node* fallback = allNodes[dist(gen) % _numRoots];
			fallback->children.push_back(pNode);
			nodeDepth[pNode] = 1;
		}

		allNodes.push_back(pNode); // 새로운 노드도 목록에 추가

		// 순환 참조
		bool connectToExisting = (rand() % 100 < 5); // 1% 확률
		if (connectToExisting && !allNodes.empty())
		{
			int sharedIndex = dist(gen) % allNodes.size();
			Node* sharedChild = allNodes[sharedIndex];

			if (sharedIndex >= _numRoots) // 루트 제외!
			{
				Node* anotherParent = allNodes[dist(gen) % allNodes.size()];
				if (anotherParent != sharedChild)
				{
					anotherParent->children.push_back(sharedChild);
					//std::cout << "Shared " << sharedChild->key << " to " << anotherParent->key << "\n";
				}
			}
		}
	}

	// 디버그용 확인 + 중복 등록 방지
	for (size_t i = 0; i < _numRoots; ++i)
	{
		//PrintGraph(allNodes[i], 0, MAX_DEPTH);
	}

	return allNodes;
}

void MarkRandomNodes(ObjectManager* _manager, size_t _numToMark)
{
	//std::random_device rd;
	//std::mt19937 gen(rd());
	//std::uniform_int_distribution<> dist(0, _manager->GetObjects().size() - 1);

	//for (size_t i = 1; i < _numToMark; ++i)
	//{
	//	int index = dist(gen);
	//	_manager->GetObjects()[index].isMarked = true; // 임의의 객체를 마킹
	//}
}

void PrintGraph(Node* _node, int _depth, int _maxDepth)
{
	if (_depth > _maxDepth)
	{
		return;
	}

	// 현재 노드 출력 (들여쓰기 포함)
	for (int i = 0; i < _depth; ++i)
	{
		std::cout << "  "; // 깊이에 따른 들여쓰기
	}
	std::cout << _node->key << " " << _node << "\n";

	// 자식 노드들에 대해 재귀 호출
	for (auto& child : _node->children)
	{
		PrintGraph(child, _depth + 1, _maxDepth);
	}
}