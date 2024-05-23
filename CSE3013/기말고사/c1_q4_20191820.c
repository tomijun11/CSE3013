#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _tree
{
	int Map[9][9];
	struct _tree* child[9];
}Tree;

int done = 0;
void Calc(Tree* Node)
{
	if (done == 1)
		return;

	int check = 0;
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			if (Node->Map[i][j] == 0)
			{
				check = 1;
				for (int k = 0; k < 9; k++)
				{
					Node->child[k] = (Tree*)malloc(sizeof(Tree));
					
					for (int p = 0; p < 9; p++)
						for (int q = 0; q < 9; q++)
							Node->child[k]->Map[p][q] = Node->Map[p][q];
					//memcpy(Node->child[k]->Map, Node->Map, sizeof(int) * 81);

					Node->child[k]->Map[i][j] = k + 1;

					int pass = 1;
					int cnt[3][10] = { 0 };

					// row
					for (int n = 0; n < 9; n++)
						cnt[0][Node->child[k]->Map[i][n]]++;
					for (int n = 1; n < 10; n++)
						if (cnt[0][n] > 1)
							pass = 0;

					// col
					for (int n = 0; n < 9; n++)
						cnt[1][Node->child[k]->Map[n][j]]++;
					for (int n = 1; n < 10; n++)
						if (cnt[1][n] > 1)
							pass = 0;

					// box
					int si = (i / 3) * 3, sj = (j / 3) * 3;
					for (int n = 0; n < 9; n++)
						cnt[2][Node->child[k]->Map[si + n / 3][sj + n % 3]]++;
					for (int n = 1; n < 10; n++)
						if (cnt[2][n] > 1)
							pass = 0;

					if (pass == 1)
					{
						Calc(Node->child[k]);
					}
				}
			}
		}

	if (check == 0)
	{
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
				printf("%d ", Node->Map[i][j]);
			if(i < 8)
				printf("\n");
		}
		done = 1;
	}
}

int main()
{
	Tree* Root = (Tree*)malloc(sizeof(Tree));
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			scanf("%d", &(Root->Map[i][j]));
	
	Calc(Root);

	return 0;
}

/* 예시 입력 데이터 입력용

0 3 5 4 6 9 2 7 8
7 8 2 1 0 5 6 0 9
0 6 0 2 7 8 1 3 5
3 2 1 0 4 6 8 9 7
8 0 4 9 1 3 5 0 6
5 9 6 8 2 0 4 1 3
9 1 7 6 5 2 0 8 0
6 0 3 7 0 1 9 5 2
2 5 8 3 9 4 7 6 0

*/