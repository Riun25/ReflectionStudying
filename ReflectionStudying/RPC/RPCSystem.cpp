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
		try // �̰� optional�� �ٲٴ� �� ã��
		{
			// �Լ� ȣ�� �� ��� ���
			std::any result = functionTable[key](_obj);
			if (result.has_value())
			{
				std::cout << "Result: " << std::any_cast<std::string>(result) << "\n"; // �̰� ���÷��ǿ��� Ÿ�� �����ؼ� �޾ƿ;� ��
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
