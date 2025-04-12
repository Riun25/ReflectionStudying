#pragma once
#include "Descriptor.h"

/* 추가한 타입------------------------------
int, float, double, char, bool, std::string
------------------------------------------*/

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
		virtual bool IsPrimitive() const override { return true; }
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<int>()
	{
		static TypeDescriptor_Int typeDesc;
		return &typeDesc;
	}

	// A type descriptor for float
	class TypeDescriptor_Float : public TypeDescriptor
	{
	public:
		TypeDescriptor_Float() : TypeDescriptor("float", sizeof(float)) {}

		virtual void Dump(const void* _obj, int /* unused */) const override
		{
			std::cout << "float{" << *(const float*)_obj << "}";
		}
		virtual bool IsPrimitive() const override { return true; }
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<float>()
	{
		static TypeDescriptor_Float typeDesc;
		return &typeDesc;
	}

	// A type descriptor for double
	class TypeDescriptor_Double : public TypeDescriptor
	{
	public:
		TypeDescriptor_Double() : TypeDescriptor("double", sizeof(double)) {}

		virtual void Dump(const void* _obj, int /* unused */) const override
		{
			std::cout << "double{" << *(const double*)_obj << "}";
		}
		virtual bool IsPrimitive() const override { return true; }
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<double>()
	{
		static TypeDescriptor_Double typeDesc;
		return &typeDesc;
	}

	// A type descriptor for char
	class TypeDescriptor_Char : public TypeDescriptor
	{
	public:
		TypeDescriptor_Char() : TypeDescriptor("char", sizeof(char)) {}

		virtual void Dump(const void* _obj, int /* unused */) const override
		{
			std::cout << "char{" << *(const char*)_obj << "}";
		}
		virtual bool IsPrimitive() const override { return true; }
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<char>()
	{
		static TypeDescriptor_Char typeDesc;
		return &typeDesc;
	}

	// A type descriptor for bool
	class TypeDescriptor_Bool : public TypeDescriptor
	{
	public:
		TypeDescriptor_Bool() : TypeDescriptor("bool", sizeof(bool)) {}

		virtual void Dump(const void* _obj, int /* unused */) const override
		{
			std::cout << "bool{" << *(const bool*)_obj << "}";
		}
		virtual bool IsPrimitive() const override { return true; }
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<bool>()
	{
		static TypeDescriptor_Bool typeDesc;
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
		virtual bool IsPrimitive() const override { return true; }
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<std::string>()
	{
		static TypeDescriptor_StdString typeDesc;
		return &typeDesc;
	}
}
