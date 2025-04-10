#pragma once
#include <iostream>
#include <string>
#include <any>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <stdexcept>

// 함수 메타데이터 저장소------------------------------------------------------------------------------------------------------------
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

// 함수 트레이트------------------------------------------------------------------------------------------------------------
template <typename T>
struct function_traits;

template <typename ReturnType, typename... Args>
struct function_traits<std::function<ReturnType(Args...)>> 
{
	using return_type = ReturnType;
	using args_tuple = std::tuple<Args...>;
	static constexpr size_t arg_count = sizeof...(Args);
};

// 멤버 함수 포인터의 경우
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...)>
{
	using return_type = ReturnType;
	using args_tuple = std::tuple<Args...>;
	static constexpr size_t arg_count = sizeof...(Args);
};

// const 멤버 함수 포인터의 경우
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> 
{
	using return_type = ReturnType;
	using args_tuple = std::tuple<Args...>;
	static constexpr size_t arg_count = sizeof...(Args);
};

// 튜플 기반 호출 래퍼
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

// 튜플의 각 요소의 타입 이름을 문자열로 반환
template <typename Tuple, std::size_t... Index>
std::vector<std::string> GetParameterTypeNamesImpl(std::index_sequence<Index...>) 
{
	return { typeid(std::tuple_element_t<Index, Tuple>).name()... };
}

template <typename Tuple>
std::vector<std::string> GetParameterTypeNames() 
{
	return GetParameterTypeNamesImpl<Tuple>(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

// 함수의 매개변수 타입을 문자열 벡터로 반환
template <typename Func>
std::vector<std::string> GetFunctionParameterTypes() 
{
	using traits = function_traits<Func>;
	using args_tuple = typename traits::args_tuple;
	return GetParameterTypeNames<args_tuple>();
}