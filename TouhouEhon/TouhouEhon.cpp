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
		//念の為UTF-16エンコで開く
		auto result = _wfopen_s(&m_fileStream, CStringW( filePath.c_str() ), L"r");
		if (result != 0)
		{
			//ファイルオープン失敗
			char message[MaxBuffLength];
			sprintf_s(message,"Cannot open file \"%s\"\n",filePath.c_str());
			throw runtime_error(message);
		}
	}

private:
	static const int MaxBuffLength = 255;

	//現在表示されているテキスト
	string m_currentText;

	FILE* m_fileStream;
};

char* ConvertUtf8ToAnsi(const string& from, char* to)
{
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

int main()
{
	Shakespeare engine;
	try
	{
		engine.OpenScriptFile("Scripts/test.txt");
	}
	catch (const runtime_error& ex)
	{
		printf(ex.what());
	}

	//main loop
	while (true)
	{
		char iBuff;
		scanf_s("%c", &iBuff,1);
		if (iBuff == '\n')
		{
			engine.Next();

			char outText[255];

			//コンソールはSJISで表示してる
			printf("%s", ConvertUtf8ToAnsi(engine.GetCurrentText(), outText));

			printf(" NL\n\n");
		}
		else if (iBuff == 'q')
		{
			return 0;
		}

	}

    return 0;
}
