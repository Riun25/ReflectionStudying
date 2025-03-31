#pragma once
#include <unordered_set>
#include <vector>
#include <iostream>

struct Node;
class reflect::TypeDescriptor;

namespace GC
{
	/*class Object
	{
	public:
		bool marked = false;
		std::vector<Object*> references;
		uint32_t id;

		Object(uint32_t _id)
			: id(_id)
		{
		}

		void AddReference(Object* _obj)
		{
			references.push_back(_obj);
			std::cout << "Object" << this->id << " now references Object " << _obj->id <<"\n";
		}
	};*/

	class GarbageCollector
	{
	public:
		GarbageCollector() {}
		~GarbageCollector() {}

	public:
		// 할당
		void Allocate(void* _obj);

		// 루트에 추가
		void AddRoot(void* _root);

		// 루트에서 삭제
		void RemoveRoot(void* _root);

		// 루트 벡터 초기화
		void ClearRoots();

		// 마킹
		void Mark(reflect::TypeDescriptor* _typeDesc);

		// 스윕
		void Sweep(reflect::TypeDescriptor* _typeDesc);

		// 콜렉트
		void Collect(reflect::TypeDescriptor* _typeDesc);

	private:
		std::unordered_set<void*> heap;
		std::vector<void*> roots;
		std::unordered_set<const void*> markedObjects;
	};
}