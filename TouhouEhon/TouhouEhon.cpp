// TouhouEhon.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"


class Shakespeare
{
public:
	void Next()
	{
		printf("�e�X�g\n1�e�X�g\n");
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
