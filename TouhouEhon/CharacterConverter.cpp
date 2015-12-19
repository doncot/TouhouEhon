#include "CharacterConverter.h"

#include<Windows.h>

char* CharacterConverter::ConvertUtf8ToSJis(const std::string& from, char* to, const size_t toLength)
{
	//�o�͐�o�b�t�@�̒����`�F�b�N�i�k�����������l���j
	if (to == nullptr || from.length() >= toLength)
	{
		return nullptr;
	}

	//utf8->utf16
	auto len = MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, 0, 0);
	auto* unicodeText = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, unicodeText, len);

	//utf16->sjis�i���m�ɂ̓f�t�H���g�̃��P�[���j
	len = WideCharToMultiByte(CP_ACP, 0, (OLECHAR*)unicodeText, -1, NULL, 0, NULL, NULL);
	auto* sjisText = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, (OLECHAR*)unicodeText, -1, sjisText, len, NULL, NULL);

	strcpy_s(to, len, sjisText);

	delete[] unicodeText;
	delete[] sjisText;

	return to;
}

char* CharacterConverter::ConvertSJisToUtf8(const std::string& from, char* to, const size_t toLength)
{
	//������̃T�C�Y�m�F
	if (to == nullptr || from.length() >= toLength)
	{
		return nullptr;
	}

	//SJIS����UTF-16�֕ϊ�
	const int nSize = ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)from.c_str(), -1, NULL, 0);

	auto buffUtf16 = new wchar_t[nSize * 2 + 2];
	::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)from.c_str(), -1, (LPWSTR)buffUtf16, nSize);

	//UTF-16����UTF8�֕ϊ�
	const int nSizeUtf8 = ::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, NULL, 0, NULL, NULL);

	auto buffUtf8 = new char[nSizeUtf8 * 2];
	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)buffUtf8, nSizeUtf8, NULL, NULL);

	strcpy_s(to, nSizeUtf8, buffUtf8);

	delete[] buffUtf16;
	delete[] buffUtf8;

	return to;
}