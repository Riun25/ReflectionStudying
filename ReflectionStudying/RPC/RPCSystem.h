#pragma once
#include <iostream>
#include <string>
#include <any>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <stdexcept>

// �Լ� ��Ÿ������ �����------------------------------------------------------------------------------------------------------------
class RPCSystem
{
public:
	using FunctionType = std::function<void(void*, const std::vector<std::any>&)>;

	static RPCSystem& GetInstance()
	{
		static RPCSystem instance;
		return instance;
	}

	void RegisterFunction(const std::string& className, const std::string& funcName, FunctionType func)
	{
		functions[className + "::" + funcName] = func;
	}

	void Invoke(const std::string& className, const std::string& funcName, void* obj, const std::vector<std::any>& args)
	{
		auto key = className + "::" + funcName;
		if (functions.find(key) == functions.end())
		{
			throw std::runtime_error("Function not found: " + key);
		}
		functions[key](obj, args);
	}

private:
	std::unordered_map<std::string, FunctionType> functions;
};

// �Լ� Ʈ����Ʈ------------------------------------------------------------------------------------------------------------
template <typename T>
struct function_traits;

// ��� �Լ� �������� ���
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...)>
{
	using return_type = ReturnType;
	using args_tuple = std::tuple<Args...>;
	static constexpr size_t arg_count = sizeof...(Args);
};

// Ʃ�� ��� ȣ�� ����
template <typename Func, typename Obj, std::size_t... Index>
auto InvokeWithTuple(Func func, Obj* obj, const std::vector<std::any>& args, std::index_sequence<Index...>)
{
	return (obj->*func)(std::any_cast<std::tuple_element_t<Index, typename function_traits<Func>::args_tuple>>(args[Index])...);
}

template <typename Func, typename Obj>
auto CallWithArgs(Func func, Obj* obj, const std::vector<std::any>& args)
{
	constexpr size_t argCount = function_traits<Func>::arg_count;
	if (args.size() != argCount)
	{
		throw std::runtime_error("Incorrect number of arguments for function");
	}
	return InvokeWithTuple(func, obj, args, std::make_index_sequence<argCount>{});
}