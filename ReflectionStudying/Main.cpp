#include <vector>
#include "Reflection/Reflect.h"
#include "GarbageCollection/GCCollector.h"

struct Node
{
	std::string key;
	int value;
	std::vector<Node> children;

	void Print() {}
	int Add(int a, int b) { return a + b; }

	REFLECT() // �� ������ ���� �ݻ縦 Ȱ��ȭ
};

struct MyStruct
{
	void Print() {}

	REFLECT()
};



int main()
{
	/*{ // �⺻ reflection�� ���� �׽�Ʈ
	//������Ʈ ����
	Node node =
	{
		"apple", 3,	{{"pineapple", 5, {}}, {"banana", 7, {}}}
	};

	// ��� Ÿ�Կ� ���� ��ũ���� ã��
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//Ÿ�� ��ũ���͸� ã�� ������ �̸� ����Ͽ� ��� ��ü�� ��ũ������ �ֿܼ� �����մϴ�.
	typeDesc->Dump(&node);

	MyStruct myS;
	reflect::TypeDescriptor* typeDesc2 = reflect::TTypeResolver<MyStruct>::Get();
	typeDesc2->Dump(&myS);
	}*/ 

	/*{ // �⺻ GC�� ���� �׽�Ʈ
	GC::GarbageCollector gc;

    // ��Ʈ ��ü ����
	GC::Object* root1 = gc.CreateObject();
	GC::Object* root2 = gc.CreateObject();
	gc.AddRoot(root1);
	gc.AddRoot(root2);

    // �ڽ� ��ü ���� �� ���� �߰�
	GC::Object* child1 = gc.CreateObject();
    root1->AddReference(child1);

	GC::Object* child2 = gc.CreateObject();
    child1->AddReference(child2);

	GC::Object* child3 = gc.CreateObject();
    root2->AddReference(child3);

    // �ڽ��� �ڽ� ����
	GC::Object* grandchild = gc.CreateObject();
    child3->AddReference(grandchild);

    // Garbage Collection ���� �� ���� ���
    std::cout << "\n--- Before Garbage Collection ---\n";

    // ù ��° ������ �÷��� ����
    gc.Collect();

    // Ư�� ���� ���� (��Ʈ���� child3 ���� ����)
    root2->references.clear();
    std::cout << "\n--- After Clearing References from Root2 ---\n";

    // �� ��° ������ �÷��� ����
    gc.Collect();
	}*/ 

	GC::GarbageCollector gc;
	// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	Node* node = new Node
	{
		"apple", 3,	{{"pineapple", 5, {}}, {"banana", 7, {}}}
	};

	gc.AddRoot(node);      // ��Ʈ�� ���
	gc.Allocate(node);     // ���� �߰�

	for (Node& child : node->children) 
	{
		gc.Allocate(&child);
	}

	// ù ��° GC ���� �� ���� ���
	std::cout << "--- Before Garbage Collection ---\n";
	typeDesc->Dump(node); // ��ũ���͸� ����Ͽ� ��� ���� ���

	gc.Collect(typeDesc); // ù ��° GC ����

	// ���� ���� �� GC ����
	node->children.clear();   // �ڽ� ���� ���� -> "pineapple"�� "banana" unreachable ���·� ����
	gc.ClearRoots();          // ��Ʈ�� ����� node�� unreachable ���·� ����

	std::cout << "\n--- After Clearing References ---\n";

	gc.Collect(typeDesc); // �� ��° GC ����

	return 0;
}

// ��� Ÿ�Կ� ���� ���Ǹ� ��Ÿ���� ��ũ��
REFLECT_STRUCT_BEGIN(Node)
REFLECT_STRUCT_MEMBER(key)
REFLECT_STRUCT_MEMBER(value)
REFLECT_STRUCT_MEMBER(children)
REFLECT_STRUCT_FUNCTION(Print, void)
REFLECT_STRUCT_FUNCTION(Add, int, int, int)
REFLECT_STRUCT_END()

REFLECT_STRUCT_BEGIN(MyStruct)
REFLECT_STRUCT_FUNCTION(Print, void)
REFLECT_STRUCT_END()