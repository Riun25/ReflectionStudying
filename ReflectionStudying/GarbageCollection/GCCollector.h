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
		// ������Ʈ ����
		Object* CreateObject();

		// ��Ʈ�� �߰�
		void AddRoot(Object* _obj);

		// ��ŷ
		void mark(Object* _obj);

		// ����
		void Sweep();

		// �ݷ�Ʈ
		void Collect();

	private:
		std::unordered_set<Object*> heap;
		std::vector<Object*> roots;
		uint32_t nextID = 0;
	};
}