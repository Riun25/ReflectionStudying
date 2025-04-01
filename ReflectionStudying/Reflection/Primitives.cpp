#pragma once
#include "Reflect.h"
#include <iostream>

namespace reflect 
{

	// A type descriptor for int
	class TypeDescriptor_Int : public TypeDescriptor
	{
	public:
		TypeDescriptor_Int() : TypeDescriptor("int", sizeof(int)){}

		virtual void Dump(const void* _obj, int /* unused */) const override
		{
			std::cout << "int{" << *(const int*)_obj << "}";
		}

		virtual void Mark(const void* /*_obj*/, std::unordered_set<const void*>& /*_markedObjects*/) const override
		{ }

		virtual void Delete(void* _obj) const override
		{ }
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<int>()
	{
		static TypeDescriptor_Int typeDesc;
		return &typeDesc;
	}

	// A type descriptor for std::string
	class TypeDescriptor_StdString : public TypeDescriptor
	{
	public:
		TypeDescriptor_StdString() :TypeDescriptor("std::string", sizeof(std::string)) {}
		
		virtual void Dump(const void* _obj, int /* unused */) const override
		{
			std::cout << "std::string{" << *(const std::string*)_obj << "}";
		}

		virtual void Mark(const void* /*_obj*/, std::unordered_set<const void*>& /*_markedObjects*/) const override
		{ }

		virtual void Delete(void* _obj) const override
		{ }
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<std::string>()
	{
		static TypeDescriptor_StdString typeDesc;
		return &typeDesc;
	}

} // namespace reflect
