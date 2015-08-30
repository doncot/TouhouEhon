// TouhouEhon.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


class Shakespeare
{
public:
	std::string Next()
	{
		return m_currentText = "テスト\n1テスト";
	}


private:
	//プロパティ
	std::string m_currentText;
};

int main()
{
	Shakespeare engine;

	while (true)
	{
		char iBuff;
		scanf_s("%c", &iBuff,1);
		if (iBuff == 'z')
		{
			engine.Next();
			printf("%s", engine.Next().c_str());
			printf(" ↓\n\n");
		}
		else if (iBuff == 'q')
		{
			return 0;
		}

	}

    return 0;
}
