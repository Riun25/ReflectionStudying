#include <vector>
#include "Reflect.h"

struct Node
{
	std::string key;
	int value;
	std::vector<Node> children;

	void Print() {}
	int Add(int a, int b) { return a + b; }

	REFLECT() // 이 유형에 대한 반사를 활성화
};

struct MyStruct
{
	void Print() {}

	REFLECT()
};



int main()
{
	//오브젝트 생성
	Node node =
	{
		"apple", 3,	{{"pineapple", 5, {}}, {"banana", 7, {}}}
	};

	// 노드 타입에 대한 디스크립터 찾기
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	//타입 디스크립터를 찾은 샘플은 이를 사용하여 노드 객체의 디스크립션을 콘솔에 덤프합니다.
	typeDesc->Dump(&node);

	MyStruct myS;
	reflect::TypeDescriptor* typeDesc2 = reflect::TTypeResolver<MyStruct>::Get();
	typeDesc2->Dump(&myS);

	return 0;
}

// 노드 타입에 대한 정의를 나타내는 매크로
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