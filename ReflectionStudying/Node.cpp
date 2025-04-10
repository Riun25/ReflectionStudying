#include "Node.h"

Node* CreateRandomGraph(ObjectManager* _manager, size_t _numNodes)
{
	//std::vector<Node*> nodes;
	//nodes.reserve(_numNodes);

	//// 루트 노드 생성
	//Node* root = new Node{ "Node_0", 0, {} };
	//nodes.push_back(root);
	//_manager->RegisterObject(static_cast<void*>(root), &Node::Reflection);

	//// 랜덤 노드 생성 및 연결
	//std::random_device rd;
	//std::mt19937 gen(rd());
	//std::uniform_int_distribution<> dist(1, _numNodes - 1);

	//for (size_t i = 1; i < _numNodes; ++i)
	//{
	//	// 새로운 노드 생성
	//	Node* node = new Node{ "Node_" + std::to_string(i), static_cast<int>(i), {} };
	//	nodes.push_back(node);
	//	_manager->RegisterObject(static_cast<void*>(node), &Node::Reflection);

	//	// 랜덤 부모 선택
	//	Node parent = nodes[dist(gen) % nodes.size()];
	//	parent->children.push_back(node); // 부모-자식 관계 설정
	//}

	//return root; // 루트 반환
	std::cout << "빈 함수임\n";
	return {};
}

std::vector<Node> CreateRandomRootGraph(ObjectManager* _manager, size_t _numRoots, size_t _numNodes)
{
	std::cout << "빈 함수임\n";
	return {};
}

void CreateRandomRootGraph(ObjectManager* _manager, size_t _numRoots, size_t _numNodes, std::unordered_set<void*>& _visited, std::vector<Node*>& _outRoots)
{
	auto& objects = _manager->GetObjects();

	_outRoots.reserve(_numRoots);

	std::vector<Node*> allNodes; // 랜덤 접근용
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
		_manager->RegisterObject(static_cast<void*>(root), &Node::Reflection, _visited);
		_outRoots.push_back(root); // 포인터로 저장
		allNodes.push_back(root); // 랜덤 접근용
	}

	// 자식 노드 생성 및 연결
	for (size_t i = _numRoots; i < _numNodes; ++i)
	{
		// 새로운 노드 생성
		Node* pNode = new Node{ "Child_" + std::to_string(i), static_cast<int>(i), {} };
		_manager->RegisterObject(static_cast<void*>(pNode), &Node::Reflection, _visited);

		constexpr int maxTries = 5;
		bool connected = false;

		for (int tries = 0; tries < maxTries; ++tries)
		{
			Node* parent = allNodes[dist(gen) % allNodes.size()]; // O(1) 랜덤 접근

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
			Node* fallback = _outRoots[dist(gen) % _outRoots.size()];
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
	for (Node* root : _outRoots)
	{
		RegisterRecursively(root, _manager, _visited);
		//PrintGraph(root, 0, MAX_DEPTH);
	}
}

void TestRandomRootGraph(ObjectManager* _manager, size_t _numRoots, size_t _numNodes, std::unordered_set<void*>& _visited)
{
	std::vector<ObjectSlot*>& objectSlots = _manager->GetpObjects();
	objectSlots.reserve(_numNodes);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, _numNodes - 1);

	// 각 노드의 깊이를 저장하는 맵 십만개 이상이므로 맵 사용
	std::unordered_map<void*, size_t> nodeDepth;

	// 랜덤 루트 노드 생성 및 연결
	for (size_t i = 0; i < _numRoots; ++i)
	{
		// 새로운 노드 생성
		Node* pRoot = new Node{ "Root_" + std::to_string(i), static_cast<int>(i), {} };
		nodeDepth[static_cast<void*>(pRoot)] = 0;
		//_manager->RegisterObject(static_cast<void*>(root), &Node::Reflection, _visited);
		ObjectSlot* objectslot = new ObjectSlot(static_cast<void*>(pRoot), &Node::Reflection); //<- 이건데... 사실 이거 RegisterObject에서 해야 함
		objectSlots.push_back(objectslot);
	}

	// 자식 노드 생성 및 연결
	for (size_t i = _numRoots; i < _numNodes; ++i)
	{
		// 새로운 노드 생성
		Node* pNode = new Node{ "Child_" + std::to_string(i), static_cast<int>(i), {} };
		//_manager->RegisterObject(static_cast<void*>(pNode), &Node::Reflection, _visited);
		ObjectSlot* objectslot = new ObjectSlot(static_cast<void*>(pNode), &Node::Reflection); //<- 이건데... 사실 이거 RegisterObject에서 해야 함
		objectSlots.push_back(objectslot);

		constexpr int maxTries = 5;
		bool connected = false;

		for (int tries = 0; tries < maxTries; ++tries)
		{
			void* parent = objectSlots[dist(gen) % objectSlots.size()]->object; // O(1) 랜덤 접근
			Node* nodeParent = static_cast<Node*>(pNode);

			if (nodeParent != pNode && nodeDepth[parent] < MAX_DEPTH)
			{
				nodeParent->children.push_back(pNode);
				nodeDepth[static_cast<void*>(pNode)] = nodeDepth[parent] + 1;
				connected = true;
				break;
			}
		}

		if (!connected)
		{
			void* vfallback = objectSlots[dist(gen) % _numRoots]->object;
			Node* fallback = static_cast<Node*>(vfallback);
			fallback->children.push_back(pNode);
			nodeDepth[static_cast<void*>(pNode)] = 1;
		}
				
		// 순환 참조
		bool connectToExisting = (rand() % 100 < 5); // 1% 확률
		if (connectToExisting && !objectSlots.empty())
		{
			int sharedIndex = dist(gen) % objectSlots.size();
			void* temp = objectSlots[sharedIndex]->object;
			Node* sharedChild = static_cast<Node*>(temp);

			if (sharedIndex >= _numRoots) // 루트 제외!
			{
				void* temp2 = objectSlots[dist(gen) % objectSlots.size()]->object;
				Node* anotherParent = static_cast<Node*>(temp2);
				if (anotherParent != sharedChild)
				{
					anotherParent->children.push_back(sharedChild);
					std::cout << "Shared " << sharedChild->key << " to " << anotherParent->key << "\n";
				}
			}
		}
	}

	// 디버그용 확인 + 중복 등록 방지
	for (size_t i = 0; i < _numRoots -1; ++i)
	{
		Node* root = static_cast<Node*>(objectSlots[i]->object);
		//RegisterRecursively(root, _manager, _visited);
		PrintGraph(root, 0, MAX_DEPTH);
	}
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

void RegisterRecursively(Node* node, ObjectManager* manager, std::unordered_set<void*>& visited)
{
	if (visited.find(node) != visited.end())
		return;

	manager->RegisterObject(node, &Node::Reflection, visited);

	for (Node* child : node->children)
	{
		RegisterRecursively(child, manager, visited);
	}
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