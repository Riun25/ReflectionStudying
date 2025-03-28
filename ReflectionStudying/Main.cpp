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

	REFLECT() // 이 유형에 대한 반사를 활성화
};

struct MyStruct
{
	void Print() {}

	REFLECT()
};



int main()
{
	/*{ // 기본 reflection에 대한 테스트
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
	}*/ 

	/*{ // 기본 GC에 대한 테스트
	GC::GarbageCollector gc;

    // 루트 객체 생성
	GC::Object* root1 = gc.CreateObject();
	GC::Object* root2 = gc.CreateObject();
	gc.AddRoot(root1);
	gc.AddRoot(root2);

    // 자식 객체 생성 및 참조 추가
	GC::Object* child1 = gc.CreateObject();
    root1->AddReference(child1);

	GC::Object* child2 = gc.CreateObject();
    child1->AddReference(child2);

	GC::Object* child3 = gc.CreateObject();
    root2->AddReference(child3);

    // 자식의 자식 생성
	GC::Object* grandchild = gc.CreateObject();
    child3->AddReference(grandchild);

    // Garbage Collection 실행 전 상태 출력
    std::cout << "\n--- Before Garbage Collection ---\n";

    // 첫 번째 가비지 컬렉션 실행
    gc.Collect();

    // 특정 참조 제거 (루트에서 child3 연결 끊기)
    root2->references.clear();
    std::cout << "\n--- After Clearing References from Root2 ---\n";

    // 두 번째 가비지 컬렉션 실행
    gc.Collect();
	}*/ 

	GC::GarbageCollector gc;
	// Node 타입에 대한 리플렉션 초기화
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	Node* node = new Node
	{
		"apple", 3,	{{"pineapple", 5, {}}, {"banana", 7, {}}}
	};

	gc.AddRoot(node);      // 루트로 등록
	gc.Allocate(node);     // 힙에 추가

	for (Node& child : node->children) 
	{
		gc.Allocate(&child);
	}

	// 첫 번째 GC 실행 전 상태 출력
	std::cout << "--- Before Garbage Collection ---\n";
	typeDesc->Dump(node); // 디스크립터를 사용하여 노드 덤프 출력

	gc.Collect(typeDesc); // 첫 번째 GC 실행

	// 참조 제거 후 GC 실행
	node->children.clear();   // 자식 참조 제거 -> "pineapple"과 "banana" unreachable 상태로 만듦
	gc.ClearRoots();          // 루트를 비워서 node도 unreachable 상태로 만듦

	std::cout << "\n--- After Clearing References ---\n";

	gc.Collect(typeDesc); // 두 번째 GC 실행

	return 0;
}

// 노드 타입에 대한 정의를 나타내는 매크로
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