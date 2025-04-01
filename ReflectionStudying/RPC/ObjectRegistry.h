#pragma once
#include <string>
#include <unordered_map>

class ObjectRegistry
{
public:
	static void RegisterObject(const std::string& _name, void* _instance)
	{
		objectMap[_name] = _instance;
	}

	static void* GetObject(const std::string& name)
	{
		auto it = objectMap.find(name);
		return (it != objectMap.end()) ? it->second : nullptr;
	}

private:
	static std::unordered_map<std::string, void*> objectMap;
};