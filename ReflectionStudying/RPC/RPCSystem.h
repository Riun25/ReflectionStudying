#pragma once
#include <any>
#include <unordered_map>
#include <functional>
#include <string>

class RPCSystem
{
public:
	RPCSystem();
	~RPCSystem();

public:
	/// �Լ� ���: Ŭ���� �̸��� �Լ� �̸����� ����
	void Register(const std::string _className, const std::string& _funcName, std::function<std::any(void*)> _func);

	// �Լ� ȣ��: ���ڿ��� Ŭ������ �Լ� �̸��� ����
	void Invoke(const std::string& _className, const std::string& _funcName, void* _obj);


	// ����


	// ���� �����

	// 



private:
	std::unordered_map<std::string, std::function<std::any(void*)>> functionTable;
};