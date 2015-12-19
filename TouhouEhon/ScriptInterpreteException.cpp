#include "ScriptInterpreteException.h"

ScriptInterpreteException::ScriptInterpreteException(const std::string& what_arg) noexcept
	:logic_error(what_arg)
{}

ScriptInterpreteException::ScriptInterpreteException(const char* what_arg) noexcept
	:logic_error(what_arg)
{}
