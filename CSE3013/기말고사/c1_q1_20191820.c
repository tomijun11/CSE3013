#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int N;
	scanf("%d", &N);

	for (int i = 1; i <= N; i++)
	{
		for (int j = 0; j < N - i; j++)
			printf(" ");

		if (N <= 2 || i == N) {
			for (int j = 0; j < 2 * i - 1; j++)
				printf("*");
		}
		else {
			for (int j = 0; j < 2 * i - 1; j++)
				if (j == 0 || j == 2 * i - 2)
					printf("*");
				else
					printf(" ");
		}

		for (int j = 0; j < N - i; j++)
			printf(" ");
		if (i < N)
			printf("\n");
	}
	

	


}