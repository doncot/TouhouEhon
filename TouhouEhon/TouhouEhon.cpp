// TouhouEhon.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

using namespace std;

class Shakespeare
{
public:
	Shakespeare() : IsEnded(false) {}

	~Shakespeare() {}

	string GetDisplay()
	{
		//空
		if (m_displayEnd <= m_displayBegin)
			return string("");

		return string(m_displayBegin, m_displayEnd);
	}

	string Next()
	{
		//最初以外は更新
		if (m_displayEnd > m_displayBegin)
			m_displayBegin = m_displayEnd + 1;

		//改ページを見つける
		regex re("\\[p\\]");
		smatch matchResult;
		const string searchScript(m_displayBegin, m_script.end());
		if (regex_search(searchScript, matchResult,re))
		{
			m_displayEnd = m_displayBegin + matchResult.position();

			//タグは消す
			m_script.erase(m_displayEnd, m_displayEnd + 2);

			return GetDisplay();
		}

		//指示タグがないなら最後までいく
		m_displayEnd = m_script.end();
		IsEnded = true;
		return GetDisplay();
	}

	void OpenScriptFile(const string& filePath)
	{
		//念の為ファイルパスはUTF-16で開く
		std::ifstream ifs( CStringW(filePath.c_str()) );
		if (ifs.fail())
		{
			//ファイルオープン失敗
			stringstream ss;
			ss << "Cannot open file \"" << filePath << "\"";
			throw runtime_error(ss.str());
		}

		std::istreambuf_iterator<char> it(ifs);
		std::istreambuf_iterator<char> last;
		m_script.assign(it, last);

		//カーソルと表示を初期位置に
		m_displayBegin = m_displayEnd = m_cursor = m_script.begin();

		//改行は全て消しておく
		m_script.erase(remove(m_script.begin(), m_script.end(), '\n'), m_script.end());
	}

	bool HasReachedEnd()
	{
		return IsEnded;
	}

private:
	static const int MaxBuffLength = 255;

	//現在表示されているテキスト
	string m_displayText;

	string m_script;
	string::iterator m_displayBegin;
	string::iterator m_displayEnd;
	//カーソル（逐次表示の表示地点）
	string::iterator m_cursor;

	//最後まで行ったか
	bool IsEnded;
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
		std::cerr << ex.what() << std::endl;
		return -1;
	}

	//main loop
	while (!engine.HasReachedEnd())
	{
		char iBuff;
		scanf_s("%c", &iBuff,1);
		if (iBuff == '\n')
		{
			//画面をクリア
			system("cls");

			engine.Next();

			char outText[255];

			//コンソールはSJISで表示してる
			printf("%s", ConvertUtf8ToAnsi(engine.GetDisplay(), outText));

			printf("\n\n>");
		}
		else if (iBuff == 'q')
		{
			return 0;
		}

	}

	printf("End of game...\n");

    return 0;
}
