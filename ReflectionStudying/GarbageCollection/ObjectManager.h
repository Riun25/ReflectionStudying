#pragma once
#include "Object.h "
#include <thread>
#include <unordered_set>
#include "../ObjectInfo.h"
#define THREAD_COUNT 8
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

	void CountMarkObject();
	std::atomic<int> markedCount = 0;

	void RegisterTypeDiscriptor(const std::string& _typeName, reflect::TypeDescriptor_SubClass* _s);
private:
	// 오브젝트 스트링으로 리플렉션 정보 찾기
	reflect::TypeDescriptor_SubClass* FindTypeDescriptor(const std::string& _typeName);
	std::vector<std::pair<std::string, reflect::TypeDescriptor_SubClass*>> testVec;
};
