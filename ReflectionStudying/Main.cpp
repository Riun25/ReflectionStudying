#include "Node.h"
#include "GarbageCollection/GCCollector.h"
#include "Timer.h"
#include <vector>


int main()
{
	GC::GarbageCollector gc;
	// Node 타입에 대한 리플렉션 초기화
	reflect::TypeDescriptor* typeDesc = reflect::TTypeResolver<Node>::Get();

	// 랜덤 그래프 생성
	Node* root = CreateRandomGraph(gc, 10);

	// 루트 설정
	gc.AddRoot(root);

	// 첫 번째 GC 실행 전 상태 출력
	std::cout << "--- Before Garbage Collection ---\n";
	typeDesc->Dump(root); // 디스크립터를 사용하여 노드 덤프 출력

	Timer* timer = new Timer();

	gc.Collect(typeDesc); // 첫 번째 GC 실행

	timer->Stop();
	std::cout << "GC Execution Time: " << timer->ElapsedMilliseconds() << " ms\n";

	// 참조 제거 후 GC 실행
	root->children.clear();   // 자식 참조 제거 -> "pineapple"과 "banana" unreachable 상태로 만듦
	gc.ClearRoots();          // 루트를 비워서 node도 unreachable 상태로 만듦

	std::cout << "\n--- After Clearing References ---\n";

	gc.Collect(typeDesc); // 두 번째 GC 실행

	delete timer;

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