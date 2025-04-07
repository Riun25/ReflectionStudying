#pragma once
#include "TestFunctions.h"
struct Node;

TestFunctions::TestFunctions()
{
	timer = new Timer();
}

TestFunctions::~TestFunctions()
{
	delete timer;
}

void TestFunctions::PrintClass()
{
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	//// ���� �׷��� ����
	//Node* node = new Node("Dog", 1, { {"Cat", 2, {}}, {"Rabbit", 3, {}} });
	//typeDesc->Dump(node);
	//delete node;
}

void TestFunctions::RunGCBasicTest(ObjectManager* _objectManager)
{
	Node* root = new Node{ "Root", 1, {new Node{"Child1", 2, {}}, new Node{"Child2", 3, {}}} };
	Node* orphan = new Node{ "Orphan", 99, {} }; // ��Ʈ���� �������� �ʴ� ��ü
	//typeDesc->Dump(root);
	//typeDesc->Dump(orphan);


	// 3. ��Ʈ ��ü �߰�
	_objectManager->RegisterObject(root, &Node::Reflection);
	_objectManager->RegisterObject(orphan, &Node::Reflection);


	// 4. GC ���� �� ���� ���
	std::cout << "--- Before Garbage Collection ---\n";
	std::cout << "Registered Objects:\n";
	for (const auto& e : _objectManager->GetObjects())
	{
		std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	}

	// 5. GC ���� (��ŷ �� ����)
	_objectManager->Mark(root);
	_objectManager->Sweep(); // true�� �����ϸ� ��ŷ�� ���� �������� ���¸� ���

	// 6. GC ���� �� ���� ���
	std::cout << "--- After Garbage Collection ---\n";

	std::cout << "Remaining Objects:\n";
	for (const auto& e : _objectManager->GetObjects())
	{
		std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	}

	//delete root; <-GC���� ó��
}

void TestFunctions::RunGCTest(ObjectManager* _objectManager, size_t _count)
{
	// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// ���� �׷��� ����
	Node* root = CreateRandomGraph(_objectManager, _count);
	//typeDesc->Dump(root); // ��ũ���͸� ����Ͽ� ��� ���� ���
	
	// ��Ʈ ����
	_objectManager->RegisterObject(root, &Node::Reflection);


	// ù ��° GC ���� �� ���� ���
	std::cout << "--- Before Garbage Collection ---\n";

	_objectManager->Mark(root);
	_objectManager->Sweep(); // ù ��° GC ����

	std::cout << "--- After Garbage Collection ---\n";

	std::cout << "Remaining Objects:\n";
	for (const auto& e : _objectManager->GetObjects())
	{
		std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	}
}

void TestFunctions::RunGCTimerTest(ObjectManager* _objectManager, size_t _count)
{
	_objectManager->ClearObject();
	
	// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// ���� �׷��� ����
	Node* root = CreateRandomGraph(_objectManager, _count);
	_objectManager->RegisterObject(root, &Node::Reflection);
	MarkRandomNodes(_objectManager, 30000);

	_objectManager->Mark(root);
	timer->Start();
	_objectManager->Sweep(); // ù ��° GC ����
	timer->Stop();
	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";
}
//
//void TestFunctions::RunGCTimerTest_LimitPerFrame(ObjectManager* _objectManager, size_t _count)
//{
//	_objectManager->ClearObject();
//	// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
//	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
//	// ���� �׷��� ����
//	Node* root = CreateRandomGraph(_objectManager, _count);
//	_objectManager->RegisterObject(root, &Node::Reflection);
//	_objectManager->AddRoot(root);
//
//	timer->Start();
//	gc.Collect_LimitPerFrame(); // ù ��° GC ����
//	timer->Stop();
//	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";
//
//	// ���� ���� �� GC ����
//	_objectManager->DeregisterObject_LimitPerFrame(root);  // �ڽ� ���� ���� -> "pineapple"�� "banana" unreachable ���·� ����
//
//	gc.Collect_LimitPerFrame(); // ù ��° GC ����
//}
//
//void TestFunctions::RunPtrTests()
//{
//	std::cout << "\n=== Testing Shared Pointer ===\n";
//	TestSharedPtr();
//
//	std::cout << "\n=== Testing Weak Pointer ===\n";
//	TestWeakPtr();
//
//	std::cout << "\n=== Testing Unique Pointer ===\n";
//	TestUniquePtr();
//
//	std::cout << "\n=== Testing Raw Pointer ===\n";
//	TestRawPtr();
//}

void TestFunctions::TestSharedPtr()
{
	//std::shared_ptr<Node> root(CreateRandomGraph(gc, 5));  // shared_ptr�� ����
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//gc.AddRoot(root.get());
	//std::cout << "--- Before GC (SharedPtr) ---\n";
	//typeDesc->Dump(root.get());

	//gc.Collect();
	//root->children.clear();  // �ڽ� ��� ����
	//gc.ClearRoots();         // ��Ʈ ����

	//std::cout << "--- After GC ---\n";
	//root.reset();            // shared_ptr ���� -> GC���� ���� ����
}

void TestFunctions::TestWeakPtr()
{
	//std::shared_ptr<Node> root(CreateRandomGraph(gc, 5));  // shared_ptr ���
	//std::weak_ptr<Node> weakRoot = root;                   // weak_ptr ����
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//gc.AddRoot(root.get());
	//std::cout << "--- Before GC (WeakPtr) ---\n";
	//typeDesc->Dump(root.get());

	//gc.Collect();
	//root->children.clear();
	//gc.ClearRoots();

	//std::cout << "--- After GC ---\n";

	//root.reset(); // shared_ptr ���� �� weak_ptr�� ����
	//if (weakRoot.expired())
	//{
	//	std::cout << "Node has been deleted.\n";
	//}
	//else
	//{
	//	typeDesc->Dump(weakRoot.lock().get());
	//}
}

void TestFunctions::TestUniquePtr()
{
	//std::unique_ptr<Node> root(CreateRandomGraph(gc, 5)); // unique_ptr ���
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//gc.AddRoot(root.get());
	//std::cout << "--- Before GC ---\n";
	//typeDesc->Dump(root.get());

	//gc.Collect();
	//root->children.clear();
	//gc.ClearRoots();

	//std::cout << "--- After GC ---\n";
	//root.reset(); // unique_ptr ����
}

void TestFunctions::TestRawPtr()
{
	//Node* root = CreateRandomGraph(gc, 5);
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//gc.AddRoot(root);
	//std::cout << "--- Before GC ---\n";
	//typeDesc->Dump(root);

	//gc.Collect();

	//root->children.clear();   // �ڽ� ���� ���� -> "pineapple"�� "banana" unreachable ���·� ����
	//gc.ClearRoots();          // ��Ʈ�� ����� node�� unreachable ���·� ����
	//std::cout << "--- After GC ---\n";

	//delete root; // ���� ������ ���� ����
}