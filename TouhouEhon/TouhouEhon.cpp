// TouhouEhon.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


class Shakespeare
{
public:
	void Next()
	{
		printf("テスト\n1テスト\n");
	}

};

int main()
{
	Shakespeare engine;

	while (1)
	{
		char iBuff;
		scanf("%c", &iBuff);
		if (iBuff == 'z')
		{
			engine.Next();
		}

	}

    return 0;
}
