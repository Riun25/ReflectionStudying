#include <vector>
#include "Reflect.h"

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

	return 0;
}

// ��� Ÿ�Կ� ���� ���Ǹ� ��Ÿ���� ��ũ��
REFLECT_STRUCT_BEGIN(Node)
REFLECT_MEMBER_START()
REFLECT_STRUCT_MEMBER(key)
REFLECT_STRUCT_MEMBER(value)
REFLECT_STRUCT_MEMBER(children)
REFLECT_MEMBER_END()
REFLECT_FUNCTION_START()
REFLECT_STRUCT_FUNCTION(Print, void)
REFLECT_STRUCT_FUNCTION(Add, int, int, int)
REFLECT_STRUCT_FUNCTION_END()

REFLECT_STRUCT_BEGIN(MyStruct)
REFLECT_FUNCTION_START()
REFLECT_STRUCT_FUNCTION(Print, void)
REFLECT_STRUCT_FUNCTION_END()