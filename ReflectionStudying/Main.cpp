#include <vector>
#include "Reflect.h"

struct Node
{
	std::string key;
	int value;
	std::vector<Node> children;

	REFLECT() // �� ������ ���� �ݻ縦 Ȱ��ȭ
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

	return 0;
}

// ��� Ÿ�Կ� ���� ���Ǹ� ��Ÿ���� ��ũ��
REFLECT_STRUCT_BEGIN(Node)
REFLECT_STRUCT_MEMBER(key)
REFLECT_STRUCT_MEMBER(value)
REFLECT_STRUCT_MEMBER(children)
REFLECT_STRUCT_END()