// TouhouEhon.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

using namespace std;

class Shakespeare
{
	struct Token;


public:
	Shakespeare() : HasEnded(false), NewPageFlag(false) {}

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
		NewPageFlag = false;

		const string searchScript(m_displayBegin, m_script.end());
		smatch matchResult;

		//改ページ or クリック待ち
		if (regex_search(searchScript, matchResult, regex(R"(\[([lp])\])")))
		{
			//改ページの場合
			if (matchResult[1] == "p")
			{
				NewPageFlag = true;
			}

			//タグ前まで行く
			m_displayEnd = m_displayBegin + matchResult.position();

			//タグは消す
			m_script.erase(m_displayEnd, m_displayEnd + 3);

			return GetDisplay();
		}

		//変数入力
		{
			stringstream pattern;
			pattern << Token::TagBegin << Token::Input << Token::WhiteSpace
				//name
				<< Token::Name << Token::OpAssign << "(" << Token::Variable << ")"
				//prompt
				"(" << Token::WhiteSpace << "prompt" << Token::OpAssign << Token::DoubleQuote << "(" << Token::String << ")" << Token::DoubleQuote << ")?"
				<< Token::TagEnd;

			if (regex_search(searchScript, matchResult, regex(pattern.str())))
			{
				//覚える
				m_variables[matchResult.str(1)] = matchResult.str(2);

				char promptMessage[255];
				CharacterConverter::ConvertUtf8ToSJis(matchResult.str(3), promptMessage,255);
				printf(">%s\n>", promptMessage);
				char inputStr[255];
				scanf_s("%s", inputStr,255);
				//入力はSJISでくる
				char sjisStr[255];
				//変数登録
				m_variables[matchResult.str(1)] = CharacterConverter::ConvertSJisToUtf8(inputStr,sjisStr,255);

				//タグを飛ばす
				//タグ前まで行く
				m_displayEnd = m_displayBegin + matchResult.position();

				//タグは消す
				m_script.erase(m_displayEnd, m_displayEnd + matchResult.length());
			}
		}

		//変数出力
		{
			stringstream pattern;
			pattern << Token::TagBegin << Token::Embedded << Token::WhiteSpace
				//exp
				<< Token::Expression << Token::OpAssign << "(" << Token::Variable << ")"
				<< Token::TagEnd;

			if (regex_search(searchScript, matchResult, regex(pattern.str())))
			{
				m_script.replace(m_displayBegin, m_displayBegin + matchResult.length(0), m_variables[matchResult.str(1)]);
			}
		}

		//指示タグがないなら最後までいく
		m_displayEnd = m_script.end();
		HasEnded = true;
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

		//改行は全て消す
		m_script.erase(remove(m_script.begin(), m_script.end(), '\n'), m_script.end());


#pragma region タグ解析
		//改行タグを改行に直す
		{
			auto temp = regex_replace(m_script, regex(R"(\[r\])"), "\n");
			m_script = temp;
		}
		
#pragma region 変数関連
		//変数を記憶
		{
			smatch match;
			regex pattern(R"(\[var ([:alpha:][[:alnum:]]*)=\"(.+)\"\])");

			if(regex_search(m_script,match,pattern))
			{
				//覚える
				m_variables[match.str(1)] = match.str(2);
			}
		}

#pragma endregion 変数関連

#pragma endregion タグ解析
	}

	bool HasReachedEnd()
	{
		return HasEnded;
	}

	bool IsNewPageNeeded()
	{
		return NewPageFlag;
	}

	void SendNewPage()
	{
		NewPageFlag = false;
		m_displayBegin = m_displayEnd;
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

	//変数
	map<string,string> m_variables;


	//flags
	bool NewPageFlag;
	//最後まで行ったか
	bool HasEnded;

#pragma region lexトークン
	struct Token
	{
		static constexpr const char *Variable = "[:alpha:][[:alnum:]]*";
		static constexpr const char *Input = "input";
		static constexpr const char *Embedded = "emb";
		static constexpr const char *Expression = "exp";
		static constexpr const char *TagBegin = R"(\[)";
		static constexpr const char *TagEnd = R"(\])";
		static constexpr const char *WhiteSpace = R"(\s)";
		static constexpr const char *Name = "name";
		static constexpr const char *OpAssign = "=";
		static constexpr const char *DoubleQuote = R"(\")";
		static constexpr const char *String = R"([^\"]*)";

	};
#pragma endregion lexトークン



};

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

	printf(">");

	//main loop
	while (!engine.HasReachedEnd())
	{
		char iBuff;
		scanf_s("%c", &iBuff,1);
		if (iBuff == '\n')
		{
			//画面をクリア
			if (engine.IsNewPageNeeded())
			{
				system("cls");
				engine.SendNewPage();
			}

			engine.Next();

			char outText[255];

			//コンソールはSJISで表示してる
			printf("%s", CharacterConverter::ConvertUtf8ToSJis(engine.GetDisplay(), outText,255));
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
