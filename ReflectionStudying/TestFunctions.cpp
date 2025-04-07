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
	//// 랜덤 그래프 생성
	//Node* node = new Node("Dog", 1, { {"Cat", 2, {}}, {"Rabbit", 3, {}} });
	//typeDesc->Dump(node);
	//delete node;
}

void TestFunctions::RunGCBasicTest(ObjectManager* _objectManager)
{
	Node* root = new Node{ "Root", 1, {new Node{"Child1", 2, {}}, new Node{"Child2", 3, {}}} };
	Node* orphan = new Node{ "Orphan", 99, {} }; // 루트에서 참조되지 않는 객체
	//typeDesc->Dump(root);
	//typeDesc->Dump(orphan);


	// 3. 루트 객체 추가
	_objectManager->RegisterObject(root, &Node::Reflection);
	_objectManager->RegisterObject(orphan, &Node::Reflection);


	// 4. GC 실행 전 상태 출력
	std::cout << "--- Before Garbage Collection ---\n";
	std::cout << "Registered Objects:\n";
	for (const auto& e : _objectManager->GetObjects())
	{
		std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	}

	// 5. GC 실행 (마킹 및 스윕)
	_objectManager->Mark(root);
	_objectManager->Sweep(); // true로 설정하면 마킹과 스윕 과정에서 상태를 출력

	// 6. GC 실행 후 상태 출력
	std::cout << "--- After Garbage Collection ---\n";

	std::cout << "Remaining Objects:\n";
	for (const auto& e : _objectManager->GetObjects())
	{
		std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	}

	//delete root; <-GC에서 처리
}

void TestFunctions::RunGCTest(ObjectManager* _objectManager, size_t _count)
{
	// Node 타입에 대한 리플렉션 초기화
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// 랜덤 그래프 생성
	Node* root = CreateRandomGraph(_objectManager, _count);
	//typeDesc->Dump(root); // 디스크립터를 사용하여 노드 덤프 출력
	
	// 루트 설정
	_objectManager->RegisterObject(root, &Node::Reflection);


	// 첫 번째 GC 실행 전 상태 출력
	std::cout << "--- Before Garbage Collection ---\n";

	_objectManager->Mark(root);
	_objectManager->Sweep(); // 첫 번째 GC 실행

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
	
	// Node 타입에 대한 리플렉션 초기화
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// 랜덤 그래프 생성
	Node* root = CreateRandomGraph(_objectManager, _count);
	_objectManager->RegisterObject(root, &Node::Reflection);
	MarkRandomNodes(_objectManager, 30000);

	_objectManager->Mark(root);
	timer->Start();
	_objectManager->Sweep(); // 첫 번째 GC 실행
	timer->Stop();
	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";
}
//
//void TestFunctions::RunGCTimerTest_LimitPerFrame(ObjectManager* _objectManager, size_t _count)
//{
//	_objectManager->ClearObject();
//	// Node 타입에 대한 리플렉션 초기화
//	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
//	// 랜덤 그래프 생성
//	Node* root = CreateRandomGraph(_objectManager, _count);
//	_objectManager->RegisterObject(root, &Node::Reflection);
//	_objectManager->AddRoot(root);
//
//	timer->Start();
//	gc.Collect_LimitPerFrame(); // 첫 번째 GC 실행
//	timer->Stop();
//	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";
//
//	// 참조 제거 후 GC 실행
//	_objectManager->DeregisterObject_LimitPerFrame(root);  // 자식 참조 제거 -> "pineapple"과 "banana" unreachable 상태로 만듦
//
//	gc.Collect_LimitPerFrame(); // 첫 번째 GC 실행
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
	//std::shared_ptr<Node> root(CreateRandomGraph(gc, 5));  // shared_ptr로 생성
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//gc.AddRoot(root.get());
	//std::cout << "--- Before GC (SharedPtr) ---\n";
	//typeDesc->Dump(root.get());

	//gc.Collect();
	//root->children.clear();  // 자식 노드 제거
	//gc.ClearRoots();         // 루트 해제

	//std::cout << "--- After GC ---\n";
	//root.reset();            // shared_ptr 해제 -> GC에서 감지 가능
}

void TestFunctions::TestWeakPtr()
{
	//std::shared_ptr<Node> root(CreateRandomGraph(gc, 5));  // shared_ptr 사용
	//std::weak_ptr<Node> weakRoot = root;                   // weak_ptr 생성
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//gc.AddRoot(root.get());
	//std::cout << "--- Before GC (WeakPtr) ---\n";
	//typeDesc->Dump(root.get());

	//gc.Collect();
	//root->children.clear();
	//gc.ClearRoots();

	//std::cout << "--- After GC ---\n";

	//root.reset(); // shared_ptr 해제 → weak_ptr만 남음
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
	//std::unique_ptr<Node> root(CreateRandomGraph(gc, 5)); // unique_ptr 사용
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//gc.AddRoot(root.get());
	//std::cout << "--- Before GC ---\n";
	//typeDesc->Dump(root.get());

	//gc.Collect();
	//root->children.clear();
	//gc.ClearRoots();

	//std::cout << "--- After GC ---\n";
	//root.reset(); // unique_ptr 해제
}

void TestFunctions::TestRawPtr()
{
	//Node* root = CreateRandomGraph(gc, 5);
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//gc.AddRoot(root);
	//std::cout << "--- Before GC ---\n";
	//typeDesc->Dump(root);

	//gc.Collect();

	//root->children.clear();   // 자식 참조 제거 -> "pineapple"과 "banana" unreachable 상태로 만듦
	//gc.ClearRoots();          // 루트를 비워서 node도 unreachable 상태로 만듦
	//std::cout << "--- After GC ---\n";

	//delete root; // 원시 포인터 수동 삭제
}