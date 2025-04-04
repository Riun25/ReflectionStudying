#pragma once
#include "TestFunctions.h"

TestFunctions::TestFunctions(GC::GarbageCollector& _gc) : gc(_gc)
{
	timer = new Timer();
}

TestFunctions::~TestFunctions()
{
	delete timer;
}

void TestFunctions::PrintClass()
{
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// ���� �׷��� ����
	Node* node = new Node("Dog", 1, { {"Cat", 2, {}}, {"Rabbit", 3, {}} });
	typeDesc->Dump(node);
	delete node;
}

void TestFunctions::RunGCTest(size_t _count)
{
	// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// ���� �׷��� ����
	Node* root = CreateRandomGraph(gc, _count);
	// ��Ʈ ����
	gc.AddRoot(root);

	// ù ��° GC ���� �� ���� ���
	std::cout << "--- Before Garbage Collection ---\n";
	typeDesc->Dump(root); // ��ũ���͸� ����Ͽ� ��� ���� ���

	gc.Collect(true); // ù ��° GC ����

	// ���� ���� �� GC ����
	root->children.clear();   // �ڽ� ���� ���� -> "pineapple"�� "banana" unreachable ���·� ����
	gc.ClearRoots();          // ��Ʈ�� ����� node�� unreachable ���·� ����

	std::cout << "\n--- After Clearing References ---\n";

	gc.Collect(true); // �� ��° GC ����
}

void TestFunctions::RunGCTimerTest(size_t _count)
{
	// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// ���� �׷��� ����
	Node* root = CreateRandomGraph(gc, _count);
	// ��Ʈ ����
	gc.AddRoot(root);

	// ù ��° GC ���� �� ���� ���
	//std::cout << "--- Before Garbage Collection ---\n";
	//typeDesc->Dump(root); // ��ũ���͸� ����Ͽ� ��� ���� ���

	timer->Start();

	gc.Collect(); // ù ��° GC ����

	timer->Stop();
	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";

	// ���� ���� �� GC ����
	root->children.clear();   // �ڽ� ���� ���� -> "pineapple"�� "banana" unreachable ���·� ����
	gc.ClearRoots();          // ��Ʈ�� ����� node�� unreachable ���·� ����

	//std::cout << "\n--- After Clearing References ---\n";

	gc.Collect(); // �� ��° GC ����
}

void TestFunctions::RunPtrTests()
{
	std::cout << "\n=== Testing Shared Pointer ===\n";
	TestSharedPtr();

	std::cout << "\n=== Testing Weak Pointer ===\n";
	TestWeakPtr();

	std::cout << "\n=== Testing Unique Pointer ===\n";
	TestUniquePtr();

	std::cout << "\n=== Testing Raw Pointer ===\n";
	TestRawPtr();
}

void TestFunctions::RunGCHeapTests()
{
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	for (int i = 0; i < 10; ++i)
	{
		std::cout << "\n=== Iteration " << i + 1 << " ===\n";

		Node* root = CreateRandomGraph(gc, 5); // ���� �׷��� ����
		gc.AddRoot(root);                      // ��Ʈ �߰�

		std::cout << "--- Before Garbage Collection ---\n";
		std::cout << "Heap size: " << gc.GetHeapSize() << "\n";

		gc.Collect(); // ������ �÷��� ����

		std::cout << "--- After Garbage Collection ---\n";
		std::cout << "Heap size: " << gc.GetHeapSize() << "\n";

		gc.ClearRoots();      // ��Ʈ �ʱ�ȭ
		gc.Collect(); // ������ �÷��� ����

		std::cout << "--- After Clearing Roots and Second Garbage Collection ---\n";
		std::cout << "Heap size: " << gc.GetHeapSize() << "\n";
	}
}

void TestFunctions::TestSharedPtr()
{
	std::shared_ptr<Node> root(CreateRandomGraph(gc, 5));  // shared_ptr�� ����
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	gc.AddRoot(root.get());
	std::cout << "--- Before GC (SharedPtr) ---\n";
	typeDesc->Dump(root.get());

	gc.Collect();
	root->children.clear();  // �ڽ� ��� ����
	gc.ClearRoots();         // ��Ʈ ����

	std::cout << "--- After GC ---\n";
	root.reset();            // shared_ptr ���� -> GC���� ���� ����
}

void TestFunctions::TestWeakPtr()
{
	std::shared_ptr<Node> root(CreateRandomGraph(gc, 5));  // shared_ptr ���
	std::weak_ptr<Node> weakRoot = root;                   // weak_ptr ����
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	gc.AddRoot(root.get());
	std::cout << "--- Before GC (WeakPtr) ---\n";
	typeDesc->Dump(root.get());

	gc.Collect();
	root->children.clear();
	gc.ClearRoots();

	std::cout << "--- After GC ---\n";

	root.reset(); // shared_ptr ���� �� weak_ptr�� ����
	if (weakRoot.expired())
	{
		std::cout << "Node has been deleted.\n";
	}
	else
	{
		typeDesc->Dump(weakRoot.lock().get());
	}
}

void TestFunctions::TestUniquePtr()
{
	std::unique_ptr<Node> root(CreateRandomGraph(gc, 5)); // unique_ptr ���
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	gc.AddRoot(root.get());
	std::cout << "--- Before GC ---\n";
	typeDesc->Dump(root.get());

	gc.Collect();
	root->children.clear();
	gc.ClearRoots();

	std::cout << "--- After GC ---\n";
	root.reset(); // unique_ptr ����
}

void TestFunctions::TestRawPtr()
{
	Node* root = CreateRandomGraph(gc, 5);
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	gc.AddRoot(root);
	std::cout << "--- Before GC ---\n";
	typeDesc->Dump(root);

	gc.Collect();

	root->children.clear();   // �ڽ� ���� ���� -> "pineapple"�� "banana" unreachable ���·� ����
	gc.ClearRoots();          // ��Ʈ�� ����� node�� unreachable ���·� ����
	std::cout << "--- After GC ---\n";

	delete root; // ���� ������ ���� ����
}