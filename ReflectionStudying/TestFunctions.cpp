#pragma once
#include "TestFunctions.h"
#include "GarbageCollection/ObjectManager.h"

struct Node;

TestFunctions::TestFunctions()
{
	timer = new Timer();
}

TestFunctions::~TestFunctions()
{
	delete timer;
}

void TestFunctions::RunGCBasicTest(ObjectManager* _objectManager)
{
	//Node* root = new Node{ "Root", 1, {new Node{"Child1", 2, {}}, new Node{"Child2", 3, {}}} };
	//Node* orphan = new Node{ "Orphan", 99, {} }; // ��Ʈ���� �������� �ʴ� ��ü
	////typeDesc->Dump(root);
	////typeDesc->Dump(orphan);


	//// 3. ��Ʈ ��ü �߰�
	////_objectManager->RegisterObject(root, &Node::Reflection);
	////_objectManager->RegisterObject(orphan, &Node::Reflection);

	//// 4. GC ���� �� ���� ���
	//std::cout << "--- Before Garbage Collection ---\n";
	//std::cout << "Registered Objects:\n";
	//for (const auto& e : _objectManager->GetObjects())
	//{
	//	std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	//}

	//// 5. GC ���� (��ŷ �� ����)
	//_objectManager->Mark(root);
	//_objectManager->Sweep();

	//// 6. GC ���� �� ���� ���
	//std::cout << "--- After Garbage Collection ---\n";

	//std::cout << "Remaining Objects:\n";
	//for (const auto& e : _objectManager->GetObjects())
	//{
	//	std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	//}

	////delete root; <-GC���� ó��
}

void TestFunctions::RunGCTest(ObjectManager* _objectManager, size_t _count)
{
	//// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	//// ���� �׷��� ����
	//Node* root = CreateRandomGraph(_objectManager, _count);
	////typeDesc->Dump(root); // ��ũ���͸� ����Ͽ� ��� ���� ���

	//// ��Ʈ ����
	////_objectManager->RegisterObject(root, &Node::Reflection);
	//TypeDisposerRegistry::Register(typeDesc, [](void* obj) { delete static_cast<Node*>(obj); });

	//// ù ��° GC ���� �� ���� ���
	//std::cout << "--- Before Garbage Collection ---\n";

	//_objectManager->Mark(root);
	//_objectManager->Sweep(); // ù ��° GC ����

	//std::cout << "--- After Garbage Collection ---\n";

	//std::cout << "Remaining Objects:\n";
	//for (const auto& e : _objectManager->GetObjects())
	//{
	//	std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	//}
}

void TestFunctions::RunGCTimerTest(ObjectManager* _objectManager, size_t _count)
{
	//_objectManager->ClearObject();

	//// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	//// ���� �׷��� ����

	//Node* root = CreateRandomGraph(_objectManager, _count);
	////_objectManager->RegisterObject(root, &Node::Reflection);
	//TypeDisposerRegistry::Register(typeDesc, [](void* obj) { delete static_cast<Node*>(obj); });
	//MarkRandomNodes(_objectManager, 50000);

	//timer->Start();
	//_objectManager->Mark(root);
	//_objectManager->Sweep(); // ù ��° GC ����
	//timer->Stop();
	//std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";
}

void TestFunctions::RunGCRootsTest(ObjectManager* _objectManager, int _count)
{
	// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// ���� �׷��� ����
	std::unordered_set<void*> visited; // �湮 Ȯ��
	size_t numRoots = 10;  // ��Ʈ ��� ����
	size_t numNodes = 100000; // ��ü ��� ����

	std::vector<Node*> allNodes = CreateRandomRootGraph(_objectManager, numRoots, numNodes, allNodes);

	_objectManager->ResetMarkCount();

	std::vector<ObjectInfo*> pNodes(numNodes);
	for (size_t i = 0; i < numNodes; ++i)
	{
		pNodes[i] = allNodes[i];
	}

	timer->Start();
#ifdef USE_MULTITHREAD
	_objectManager->MarkwithThread("Node", &pNodes, _count);
#else
	for (size_t i = 0; i < _count; ++i)
	{
		_objectManager->Mark("Node", pNodes[i]);
	}
#endif

#ifdef DEVIDE_PRINT
	timer->Stop();
	std::cout << "GC Mark Time: " << timer->ElapsedMilliseconds() << " ms\n";
	
	timer->Start();
#else
#endif
	_objectManager->PrintMarkedNum();
#ifdef USE_MULTITHREAD
	_objectManager->SweepwithThread(&pNodes);
#else
	_objectManager->Sweep(&pNodes);
#endif

	timer->Stop();
#ifdef DEVIDE_PRINT
	std::cout << "GC Sweep Time: " << timer->ElapsedMilliseconds() << " ms\n";
#else
	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";
#endif
}
