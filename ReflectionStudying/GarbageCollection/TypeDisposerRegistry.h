#pragma once
#include <unordered_map>
//#include <functional>
#include "../Reflection/Container.h"

class TypeDisposerRegistry
{
public:
	using DisposerFunc = void(*)(void*);//std::function<void(void*)>;

	static void Register(reflect::TypeDescriptor* type, DisposerFunc func) 
	{
		GetMap()[type] = func;
	}

	static void Dispose(reflect::TypeDescriptor* type, void* obj) 
	{
		if (!obj || !type) return;

		auto it = GetMap().find(type);
		if (it != GetMap().end()) 
		{
			it->second(obj);
		}
		/*else
		{
			std::cerr << "[Dispose] No disposer registered for this type!\n";
		}*/
	}

private:
	static std::unordered_map<reflect::TypeDescriptor*, DisposerFunc>& GetMap()
	{
		static std::unordered_map<reflect::TypeDescriptor*, DisposerFunc> map;
		return map;
	}

};

