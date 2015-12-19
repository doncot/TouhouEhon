// TouhouEhon.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

using namespace std;

class Shakespeare
{
	struct Token;

public:
	Shakespeare() {}

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
		m_newPageFlag = false;

		const string searchScript(m_displayBegin, m_script.end());
		smatch matchedTag;

		//まずタグがあるか調べる
		if (regex_search(searchScript, matchedTag, regex(R"(\[([^\]]+)\])")))
		{
			auto tagName = matchedTag.str(1);
			smatch matchedResult;

#pragma region 改ページ or クリック待ち
			if (regex_search(tagName, matchedResult, regex(R"(^[lp]$)")))
			{
				//改ページの場合
				if (matchedResult.str(0) == "p")
				{
					m_newPageFlag = true;
				}

				//タグ前まで行く
				m_displayEnd = m_displayBegin + matchedTag.position();

				//タグは消す
				m_script.erase(m_displayEnd, m_displayEnd + 3);

				return GetDisplay();
			}
#pragma endregion

#pragma region 変数プロンプト入力
			{
				stringstream pattern;
				pattern << Token::TagBegin << Token::Input << Token::WhiteSpace
					//name
					<< Token::Name << Token::OpAssign << "(" << Token::Variable << ")"
					//prompt
					"(" << Token::WhiteSpace << "prompt" << Token::OpAssign << Token::DoubleQuote << "(" << Token::String << ")" << Token::DoubleQuote << ")?"
					<< Token::TagEnd;

				if (regex_search(searchScript, matchedResult, regex(pattern.str())))
				{
					//タグを飛ばす
					//タグ前まで行く
					m_displayEnd = m_displayBegin + matchedResult.position();

					//タグは消す
					m_script.erase(m_displayEnd, m_displayEnd + matchedResult.length());

					m_waitingInputVariable = matchedResult.str(1);

					return string("");
				}
			}
#pragma endregion

#pragma region 変数出力
			{
				stringstream pattern;
				pattern << Token::TagBegin << Token::Embedded << Token::WhiteSpace
					//exp
					<< Token::Expression << Token::OpAssign << "(" << Token::Variable << ")"
					<< Token::TagEnd;

				if (regex_search(searchScript, matchedResult, regex(pattern.str())))
				{
					m_script.replace(m_displayBegin + matchedTag.position(),
						m_displayBegin + matchedTag.position() + matchedResult.length(0),
						m_variables[matchedResult.str(1)]);

					return GetDisplay();
				}
			}
#pragma endregion

			//未定義タグ
			{
				char temp[InputBuffLength];
				CharacterConverter::ConvertUtf8ToSJis(matchedResult.str(), temp, InputBuffLength);
				stringstream exMessage;
				exMessage  << "Undefined tag (" << temp << ")";
				throw ScriptInterpreteException(exMessage.str());
			}
		}

		//指示タグがないなら最後までいく
		m_displayEnd = m_script.end();
		m_hasEnded = true;
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

		//（改行の前に）;スタイルのコメント
		{
			auto temp = regex_replace(m_script, regex(";.*"), "");
			m_script = temp;
		}

		//改行は全て消す
		m_script.erase(remove(m_script.begin(), m_script.end(), '\n'), m_script.end());


#pragma region タグ解析
		//改行タグを改行に直す
		{
			auto temp = regex_replace(m_script, regex(R"(\[r\])"), "\n");
			m_script = temp;
		}
		
#pragma endregion タグ解析
	}

	void RegisterVariable(const string& assignment)
	{
		//覚える
		m_variables[m_waitingInputVariable] = assignment;

		//前の値はクリア
		m_waitingInputVariable.clear();
	}

	bool HasReachedEnd() noexcept
	{
		return m_hasEnded;
	}

	bool IsNewPageNeeded() noexcept
	{
		return m_newPageFlag;
	}

	bool IsWaitingForUserToInputVariable()
	{
		return !m_waitingInputVariable.empty();
	}

	void SendNewPage()
	{
		m_newPageFlag = false;
		m_displayBegin = m_displayEnd;
	}

private:
	static const int InputBuffLength = 1024;

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
	bool m_newPageFlag = false;
	//最後まで行ったか
	bool m_hasEnded = false;
	//ユーザーからの操作要求がいるか
	string m_waitingInputVariable;

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

			try
			{
				engine.Next();
			}
			catch (const ScriptInterpreteException& ex)
			{
				fprintf(stderr, "ScriptInterpreteException!: %s\n", ex.what());
				scanf_s("%c",&iBuff,1);
#ifdef DEBUG
				exit(1);
#endif
			}

			//入力要求がある場合
			if (engine.IsWaitingForUserToInputVariable())
			{
				printf("ENTER:");
				char inputStr[512];
				scanf_s("%s", inputStr, 512);
				//入力はSJISでくる
				char utf8Str[512];
				CharacterConverter::ConvertSJisToUtf8(inputStr, utf8Str, 512);

				//変数登録
				engine.RegisterVariable(utf8Str);

#ifdef DEBUG
				fprintf(stderr, "registered: %s\n", inputStr);
#endif

				continue;
			}
			
			//出力
			{
				char outText[1024];
				//コンソールはSJISで表示してる
				printf("%s", CharacterConverter::ConvertUtf8ToSJis(engine.GetDisplay(), outText, 1024));
				printf("\n\n>");
			}
		}
		else if (iBuff == 'q')
		{
			return 0;
		}

	}

	printf("End of game...\n");
	char iBuff;
	scanf_s("%c", &iBuff, 1);

    return 0;
}
