#include "RPCSystem.h"
#include <iostream>

RPCSystem::RPCSystem()
{
}

RPCSystem::~RPCSystem()
{
}

void RPCSystem::Register(const std::string _className, const std::string& _funcName, std::function<std::any(void*)> _func)
{
	std::string key = _className + "." + _funcName;
	functionTable[key] = _func;
}

void RPCSystem::Invoke(const std::string& _className, const std::string& _funcName, void* _obj)
{
	std::string key = _className + "." + _funcName;
	if (functionTable.find(key) != functionTable.end())
	{
		try // 이거 optional로 바꾸는 법 찾기
		{
			// 함수 호출 및 결과 출력
			std::any result = functionTable[key](_obj);
			if (result.has_value())
			{
				std::cout << "Result: " << std::any_cast<std::string>(result) << "\n"; // 이거 리플렉션에서 타입 추정해서 받아와야 함
			}
			else
			{
				std::cout << "Function executed successfully but returned no value.\n";
			}
		}
		catch (const std::bad_any_cast& e)
		{
			std::cerr << "Error casting result: " << e.what() << "\n";
		}
	}
	else
	{
		std::cout << "Function not found: " << key << "\n";
	}
}
