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
		// �Ҵ�
		void Allocate(void* _obj);

		// ��Ʈ�� �߰�
		void AddRoot(void* _root);

		// ��Ʈ���� ����
		void RemoveRoot(void* _root);

		// ��Ʈ ���� �ʱ�ȭ
		void ClearRoots();

		// ��ŷ
		void Mark(reflect::TypeDescriptor* _typeDesc);

		// ����
		void Sweep(reflect::TypeDescriptor* _typeDesc);

		// �ݷ�Ʈ
		void Collect(reflect::TypeDescriptor* _typeDesc);

	private:
		std::unordered_set<void*> heap;
		std::vector<void*> roots;
		std::unordered_set<const void*> markedObjects;
	};
}