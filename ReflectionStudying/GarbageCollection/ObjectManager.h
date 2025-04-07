#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include "../Reflection/Container.h"

class ObjectManager
{
public:
	struct ObjectMetaData
	{
		void* object;
		bool isMarked;
		reflect::TypeDescriptor* type;

		ObjectMetaData(void* _obj, reflect::TypeDescriptor* _typeDesc)
			: isMarked(false), type(_typeDesc), object(_obj) {}
	};

	ObjectManager() {}
	~ObjectManager() {}

public:
	void RegisterObject(void* _obj, reflect::TypeDescriptor* _typeDesc);
	void Mark(void* _root);
	void Sweep();

	std::vector<ObjectMetaData>& GetObjects() { return objects; }
	void ClearObject() { objects.clear(); }

private:
	std::vector<ObjectMetaData> objects; // ���� ���� ��ü ���
	const int threadCount =12; // ����� ������ ����
	void SweepWithErase();
	void SweepWithNewVector();
	void SweepWithThreads();
};
