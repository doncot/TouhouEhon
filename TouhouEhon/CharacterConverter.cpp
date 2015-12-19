#include "CharacterConverter.h"

#include<Windows.h>

char* CharacterConverter::ConvertUtf8ToSJis(const std::string& from, char* to, const size_t toLength)
{
	//出力先バッファの長さチェック（ヌル文字分も考慮）
	if (to == nullptr || from.length() >= toLength)
	{
		return nullptr;
	}

	//utf8->utf16
	auto len = MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, 0, 0);
	auto* unicodeText = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, unicodeText, len);

	//utf16->sjis（正確にはデフォルトのロケール）
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
	//割当先のサイズ確認
	if (to == nullptr || from.length() >= toLength)
	{
		return nullptr;
	}

	//SJISからUTF-16へ変換
	const int nSize = ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)from.c_str(), -1, NULL, 0);

	auto buffUtf16 = new wchar_t[nSize * 2 + 2];
	::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)from.c_str(), -1, (LPWSTR)buffUtf16, nSize);

	//UTF-16からUTF8へ変換
	const int nSizeUtf8 = ::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, NULL, 0, NULL, NULL);

	auto buffUtf8 = new char[nSizeUtf8 * 2];
	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)buffUtf8, nSizeUtf8, NULL, NULL);

	strcpy_s(to, nSizeUtf8, buffUtf8);

	delete[] buffUtf16;
	delete[] buffUtf8;

	return to;
}