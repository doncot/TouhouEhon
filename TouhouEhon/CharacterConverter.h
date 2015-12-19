#pragma once
#include<string>

class CharacterConverter
{
public:

	static char* ConvertUtf8ToSJis(const std::string& from, char* to, size_t toLength);

	static char* ConvertSJisToUtf8(const std::string& from, char* to, size_t toLength);
};

