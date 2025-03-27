#pragma once
#include <unordered_set>
#include <vector>
#include <iostream>

namespace GC
{
	class Object
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
	};

	class GarbageCollector
	{
	public:
		GarbageCollector() {}
		~GarbageCollector() {}

	public:
		// 오브젝트 생성
		Object* CreateObject();

		// 루트에 추가
		void AddRoot(Object* _obj);

		// 마킹
		void mark(Object* _obj);

		// 스윕
		void Sweep();

		// 콜렉트
		void Collect();

	private:
		std::unordered_set<Object*> heap;
		std::vector<Object*> roots;
		uint32_t nextID = 0;
	};
}