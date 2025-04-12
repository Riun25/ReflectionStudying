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
	//Node* orphan = new Node{ "Orphan", 99, {} }; // 루트에서 참조되지 않는 객체
	////typeDesc->Dump(root);
	////typeDesc->Dump(orphan);


	//// 3. 루트 객체 추가
	////_objectManager->RegisterObject(root, &Node::Reflection);
	////_objectManager->RegisterObject(orphan, &Node::Reflection);

	//// 4. GC 실행 전 상태 출력
	//std::cout << "--- Before Garbage Collection ---\n";
	//std::cout << "Registered Objects:\n";
	//for (const auto& e : _objectManager->GetObjects())
	//{
	//	std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	//}

	//// 5. GC 실행 (마킹 및 스윕)
	//_objectManager->Mark(root);
	//_objectManager->Sweep();

	//// 6. GC 실행 후 상태 출력
	//std::cout << "--- After Garbage Collection ---\n";

	//std::cout << "Remaining Objects:\n";
	//for (const auto& e : _objectManager->GetObjects())
	//{
	//	std::cout << "  Object: " << e.object << ", Type: " << e.type->GetFullName() << "\n";
	//}

	////delete root; <-GC에서 처리
}

void TestFunctions::RunGCTest(ObjectManager* _objectManager, size_t _count)
{
	//// Node 타입에 대한 리플렉션 초기화
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	//// 랜덤 그래프 생성
	//Node* root = CreateRandomGraph(_objectManager, _count);
	////typeDesc->Dump(root); // 디스크립터를 사용하여 노드 덤프 출력

	//// 루트 설정
	////_objectManager->RegisterObject(root, &Node::Reflection);
	//TypeDisposerRegistry::Register(typeDesc, [](void* obj) { delete static_cast<Node*>(obj); });

	//// 첫 번째 GC 실행 전 상태 출력
	//std::cout << "--- Before Garbage Collection ---\n";

	//_objectManager->Mark(root);
	//_objectManager->Sweep(); // 첫 번째 GC 실행

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

	//// Node 타입에 대한 리플렉션 초기화
	//reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	//// 랜덤 그래프 생성

	//Node* root = CreateRandomGraph(_objectManager, _count);
	////_objectManager->RegisterObject(root, &Node::Reflection);
	//TypeDisposerRegistry::Register(typeDesc, [](void* obj) { delete static_cast<Node*>(obj); });
	//MarkRandomNodes(_objectManager, 50000);

	//timer->Start();
	//_objectManager->Mark(root);
	//_objectManager->Sweep(); // 첫 번째 GC 실행
	//timer->Stop();
	//std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";
}

void TestFunctions::RunGCRootsTest(ObjectManager* _objectManager, int _count)
{
	// Node 타입에 대한 리플렉션 초기화
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();
	// 랜덤 그래프 생성
	std::unordered_set<void*> visited; // 방문 확인
	size_t numRoots = 10;  // 루트 노드 개수
	size_t numNodes = 100000; // 전체 노드 개수

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
