#pragma once
#include "../RPC/RPCSystem.h"
#include "Container.h"
#include "Reflection/Reference.h"
#include <unordered_set>
#include <iostream>

// 리플렉션 시스템
namespace reflect 
{
	// 리플렉션 매크로
#define REFLECT() \
	friend class reflect::DefaultResolver; \
	static reflect::TypeDescriptor_SubClass Reflection; \
	static void initReflection(reflect::TypeDescriptor_SubClass*);

#define REFLECT_STRUCT_BEGIN(_type) \
	reflect::TypeDescriptor_SubClass _type::Reflection{ _type::initReflection }; \
	void _type::initReflection(reflect::TypeDescriptor_SubClass* _typeDesc) { \
		using T = _type; \
		_typeDesc->name = #_type; \
		_typeDesc->size = sizeof(T); \
		_typeDesc->memberVec.clear(); \
		_typeDesc->functionVec.clear();

#define REFLECT_STRUCT_MEMBER(_name) \
		_typeDesc->memberVec.push_back({#_name, offsetof(T, _name), reflect::TTypeResolver<decltype(T::_name)>::Get()});

#define REFLECT_STRUCT_FUNCTION(_type, _func) \
		_typeDesc->functionVec.push_back({#_func, GetFunctionParameterTypes<decltype(&_type::_func)>(),\
		[](void* obj, const std::vector<std::any>& args) { \
			CallWithArgs(&_type::_func, static_cast<_type*>(obj), args); \
		}});

#define REFLECT_STRUCT_END() \
		for (const auto& func : _typeDesc->functionVec) { \
			RPCSystem::GetInstance().RegisterFunction(_typeDesc->name, func.name, func.invoker); \
		} \
		GC::GarbageCollector::GetInstance().RegisterType(_typeDesc); /* GC 등록 */ \
	}

} // namespace reflect