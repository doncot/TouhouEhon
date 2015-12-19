#pragma once
#include <stdexcept>
#include <string>

//スクリプトの論理エラー
class ScriptInterpreteException :
	public std::logic_error
{
public:
	explicit ScriptInterpreteException(const std::string& what_arg) noexcept;
	explicit ScriptInterpreteException(const char* what_arg) noexcept;
};

