#pragma once
#include "Object.h "
#include <thread>
#include <queue>
#include <unordered_set>

class ObjectManager
{
public:
	ObjectManager() {}
	~ObjectManager() {}

public:
	void RegisterObject(void* _obj, reflect::TypeDescriptor* _typeDesc, std::unordered_set<void*>& _visited);
	void MarkAll(const std::vector<void*>& roots);
	void Mark(void* _root);
	void MarkTest(int _numRoot);
	void Sweep();

	std::vector<ObjectSlot>& GetObjects() { return objects; }
	std::vector<ObjectSlot*>& GetpObjects() { return objects2; }
	void ClearObject() { objects.clear(); }
	void PrintObject();
	void CountMarkObject();

	void FilledObjIdx();
private:

	std::vector<ObjectSlot> objects; // ���� ���� ��ü ���
	std::vector<ObjectSlot*> objects2; // ���� ���� ��ü ���
	const int threadCount = 16; // ����� ������ ����
	int markedCount = 0;
	void SweepWithErase();
	void SweepWithNewVector(int _markedCount);
	void SweepWithThreads();
	std::unordered_map<void*, ObjectSlot*> objectIndex; // <- ��ŷ �ӵ��� ����ȭ�ϱ� ���� ����
};
