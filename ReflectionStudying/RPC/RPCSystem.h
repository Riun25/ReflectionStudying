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
	/// 함수 등록: 클래스 이름과 함수 이름으로 매핑
	void Register(const std::string _className, const std::string& _funcName, std::function<std::any(void*)> _func);

	// 함수 호출: 문자열로 클래스와 함수 이름을 지정
	void Invoke(const std::string& _className, const std::string& _funcName, void* _obj);


	// 삭제


	// 완전 지우기

	// 



private:
	std::unordered_map<std::string, std::function<std::any(void*)>> functionTable;
};