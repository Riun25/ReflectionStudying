#include <vector>
#include "Reflect.h"

struct Node
{
	std::string key;
	int value;
	std::vector<Node> children;

	REFLECT() // 이 유형에 대한 반사를 활성화
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

	return 0;
}

// 노드 타입에 대한 정의를 나타내는 매크로
REFLECT_STRUCT_BEGIN(Node)
REFLECT_STRUCT_MEMBER(key)
REFLECT_STRUCT_MEMBER(value)
REFLECT_STRUCT_MEMBER(children)
REFLECT_STRUCT_END()