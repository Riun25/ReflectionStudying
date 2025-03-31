#include "Node.h"
#include "GarbageCollection/GCCollector.h"
#include "Timer.h"
#include <vector>


int main()
{
	GC::GarbageCollector gc;
	// Node Ÿ�Կ� ���� ���÷��� �ʱ�ȭ
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	// ���� �׷��� ����
	Node* root = CreateRandomGraph(gc, 10);

	// ��Ʈ ����
	gc.AddRoot(root);

	// ù ��° GC ���� �� ���� ���
	std::cout << "--- Before Garbage Collection ---\n";
	typeDesc->Dump(root); // ��ũ���͸� ����Ͽ� ��� ���� ���

	Timer* timer = new Timer();

	gc.Collect(typeDesc); // ù ��° GC ����

	timer->Stop();
	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";

	// ���� ���� �� GC ����
	root->children.clear();   // �ڽ� ���� ���� -> "pineapple"�� "banana" unreachable ���·� ����
	gc.ClearRoots();          // ��Ʈ�� ����� node�� unreachable ���·� ����

	std::cout << "\n--- After Clearing References ---\n";

	gc.Collect(typeDesc); // �� ��° GC ����

	delete timer;

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