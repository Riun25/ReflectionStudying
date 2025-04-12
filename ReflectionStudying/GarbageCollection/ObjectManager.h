#pragma once
#include <thread>
#include "../ObjectInfo.h"
#include "../Reflection/Descriptor.h"

struct Node;

class ObjectManager
{
public:
	ObjectManager() {}
	~ObjectManager() {}

public:
	void Mark(const std::string& _typeName, ObjectInfo* _obj);
	void Sweep(std::vector<ObjectInfo*>* _nodes);
	void MarkwithThread(const std::string& _typeName, std::vector<ObjectInfo*>* _nodes, int _count);
	void SweepwithThread(std::vector<ObjectInfo*>* _nodes);

	void PrintMarkedNum();
	void ResetMarkCount() {markedCount = 0;}
	void RegisterTypeDiscriptor(const std::string& _typeName, reflect::TypeDescriptor_SubClass* _s);

private:
	std::atomic<int> markedCount = 0;
	// ������Ʈ ��Ʈ������ ���÷��� ���� ã��
	reflect::TypeDescriptor_SubClass* FindTypeDescriptor(const std::string& _typeName);
	std::vector<std::pair<std::string, reflect::TypeDescriptor_SubClass*>> testVec;
};
