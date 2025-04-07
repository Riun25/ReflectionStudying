#pragma once
#include "TestFunctions.h"
#include "GarbageCollection/TypeDisposerRegistry.h"

class TypeDisposerRegistry;
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
	TypeDisposerRegistry::Register(typeDesc, [](void* obj) { delete static_cast<Node*>(obj); });

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
	TypeDisposerRegistry::Register(typeDesc, [](void* obj) { delete static_cast<Node*>(obj); });
	
	MarkRandomNodes(_objectManager, 30000);

	_objectManager->Mark(root);
	timer->Start();
	_objectManager->Sweep(); // ù ��° GC ����
	timer->Stop();
	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";
}