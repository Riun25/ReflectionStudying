#pragma once
#include "Descriptor.h"
#include <memory>
#include <vector>

namespace reflect
{
	class DefaultResolver
	{
	public:
		template <typename T>
		struct IsReflected
		{
			static constexpr bool value = requires
			{
				&T::Reflection;
			};
		};

		template <typename T>
		static constexpr auto Get() -> std::enable_if_t<IsReflected<T>::value, TypeDescriptor*>
		{
			return &T::Reflection;
		}

		template <typename T>
		static constexpr auto Get() -> std::enable_if_t<!IsReflected<T>::value, TypeDescriptor*>
		{
			return GetPrimitiveDescriptor<T>();
		}
	};

	template <typename T>
	class TTypeResolver
	{
	public:
		static TypeDescriptor* Get()
		{
			return DefaultResolver::Get<T>();
		}
	};

	// Ư��ȭ�� ������ �ٸ� ���Ͽ� ������ �˸�

	//A type descriptor for vector<>
	template <typename T>
	class TTypeResolver<std::vector<T>>;

	//A type descriptor for unique_ptr<>
	template <typename T>
	class TTypeResolver<std::unique_ptr<T>>;
}