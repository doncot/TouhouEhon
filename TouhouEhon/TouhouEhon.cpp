// TouhouEhon.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

using namespace std;

class Shakespeare
{
public:
	~Shakespeare()
	{
		if (m_fileStream)
		{
			fclose(m_fileStream);
		}
	}

	string GetCurrentText()
	{
		return m_currentText;
	}

	string Next()
	{
		//return m_currentText = "テスト\n1テスト";

		char buff[MaxBuffLength];
		while ( fgets(buff,MaxBuffLength,m_fileStream) != nullptr )
		{
			m_currentText.append(buff);
		}

		return m_currentText;
	}

	void OpenScriptFile(const string& filePath)
	{
		//m_fileStream = fopen(filePath.c_str(), "r+t,ccs=UTF-8");
		m_fileStream = fopen(filePath.c_str(), "r");
		if (m_fileStream == nullptr)
		{
			printf("Cannot open %s\n", filePath.c_str());
		}
	}

private:
	static const int MaxBuffLength = 255;

	//現在表示されているテキスト
	string m_currentText;

	FILE* m_fileStream;
};

char* ConvertUtf8ToSjis(const string& from, char* to)
{
	//utf8->utf16
	auto len = MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, 0, 0);
	auto* unicodeText = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, unicodeText, len);

	//utf16->sjis
	len = WideCharToMultiByte(CP_ACP, 0, (OLECHAR*)unicodeText, -1, NULL, 0, NULL, NULL);
	auto* sjisText = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, (OLECHAR*)unicodeText, -1, sjisText, len, NULL, NULL);

	strcpy_s(to, len, sjisText);

	delete[] unicodeText;
	delete[] sjisText;

	return to;
}

int main()
{
	Shakespeare engine;
	engine.OpenScriptFile("Scripts/test.txt");

	while (true)
	{
		char iBuff;
		scanf_s("%c", &iBuff,1);
		if (iBuff == 'z')
		{
			engine.Next();

			char outText[255];

			//コンソールはSJISで表示してる
			printf("%s", ConvertUtf8ToSjis(engine.GetCurrentText(),outText));

			printf(" ↓\n\n");
		}
		else if (iBuff == 'q')
		{
			return 0;
		}

	}

    return 0;
}
