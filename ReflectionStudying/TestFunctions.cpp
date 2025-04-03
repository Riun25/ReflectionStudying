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
	// 랜덤 그래프 생성
	Node* node = new Node("Dog", 1, { {"Cat", 2, {}}, {"Rabbit", 3, {}} });
	typeDesc->Dump(node);
	delete node;
}

void TestFunctions::RunGCTest(size_t _count)
{
	// Node 타입에 대한 리플렉션 초기화
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// 랜덤 그래프 생성
	Node* root = CreateRandomGraph(gc, _count);
	// 루트 설정
	gc.AddRoot(root);

	// 첫 번째 GC 실행 전 상태 출력
	std::cout << "--- Before Garbage Collection ---\n";
	typeDesc->Dump(root); // 디스크립터를 사용하여 노드 덤프 출력

	gc.Collect(true); // 첫 번째 GC 실행

	// 참조 제거 후 GC 실행
	root->children.clear();   // 자식 참조 제거 -> "pineapple"과 "banana" unreachable 상태로 만듦
	gc.ClearRoots();          // 루트를 비워서 node도 unreachable 상태로 만듦

	std::cout << "\n--- After Clearing References ---\n";

	gc.Collect(true); // 두 번째 GC 실행
}

void TestFunctions::RunGCTimerTest(size_t _count)
{
	// Node 타입에 대한 리플렉션 초기화
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// 랜덤 그래프 생성
	Node* root = CreateRandomGraph(gc, _count);
	// 루트 설정
	gc.AddRoot(root);

	// 첫 번째 GC 실행 전 상태 출력
	//std::cout << "--- Before Garbage Collection ---\n";
	//typeDesc->Dump(root); // 디스크립터를 사용하여 노드 덤프 출력

	timer->Start();

	gc.Collect(); // 첫 번째 GC 실행

	timer->Stop();
	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";

	// 참조 제거 후 GC 실행
	root->children.clear();   // 자식 참조 제거 -> "pineapple"과 "banana" unreachable 상태로 만듦
	gc.ClearRoots();          // 루트를 비워서 node도 unreachable 상태로 만듦

	//std::cout << "\n--- After Clearing References ---\n";

	gc.Collect(); // 두 번째 GC 실행
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

		Node* root = CreateRandomGraph(gc, 5); // 랜덤 그래프 생성
		gc.AddRoot(root);                      // 루트 추가

		std::cout << "--- Before Garbage Collection ---\n";
		std::cout << "Heap size: " << gc.GetHeapSize() << "\n";

		gc.Collect(); // 가비지 컬렉션 실행

		std::cout << "--- After Garbage Collection ---\n";
		std::cout << "Heap size: " << gc.GetHeapSize() << "\n";

		gc.ClearRoots();      // 루트 초기화
		gc.Collect(); // 가비지 컬렉션 실행

		std::cout << "--- After Clearing Roots and Second Garbage Collection ---\n";
		std::cout << "Heap size: " << gc.GetHeapSize() << "\n";
	}
}

void TestFunctions::TestSharedPtr()
{
	std::shared_ptr<Node> root(CreateRandomGraph(gc, 5));  // shared_ptr로 생성
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	gc.AddRoot(root.get());
	std::cout << "--- Before GC (SharedPtr) ---\n";
	typeDesc->Dump(root.get());

	gc.Collect();
	root->children.clear();  // 자식 노드 제거
	gc.ClearRoots();         // 루트 해제

	std::cout << "--- After GC ---\n";
	root.reset();            // shared_ptr 해제 -> GC에서 감지 가능
}

void TestFunctions::TestWeakPtr()
{
	std::shared_ptr<Node> root(CreateRandomGraph(gc, 5));  // shared_ptr 사용
	std::weak_ptr<Node> weakRoot = root;                   // weak_ptr 생성
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	gc.AddRoot(root.get());
	std::cout << "--- Before GC (WeakPtr) ---\n";
	typeDesc->Dump(root.get());

	gc.Collect();
	root->children.clear();
	gc.ClearRoots();

	std::cout << "--- After GC ---\n";

	root.reset(); // shared_ptr 해제 → weak_ptr만 남음
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
	std::unique_ptr<Node> root(CreateRandomGraph(gc, 5)); // unique_ptr 사용
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	gc.AddRoot(root.get());
	std::cout << "--- Before GC ---\n";
	typeDesc->Dump(root.get());

	gc.Collect();
	root->children.clear();
	gc.ClearRoots();

	std::cout << "--- After GC ---\n";
	root.reset(); // unique_ptr 해제
}

void TestFunctions::TestRawPtr()
{
	Node* root = CreateRandomGraph(gc, 5);
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	gc.AddRoot(root);
	std::cout << "--- Before GC ---\n";
	typeDesc->Dump(root);

	gc.Collect();

	root->children.clear();   // 자식 참조 제거 -> "pineapple"과 "banana" unreachable 상태로 만듦
	gc.ClearRoots();          // 루트를 비워서 node도 unreachable 상태로 만듦
	std::cout << "--- After GC ---\n";

	delete root; // 원시 포인터 수동 삭제
}