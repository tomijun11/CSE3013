#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _move {
	int dy;
	int dx;
}Move;

Move M[8] = { // Y X ¼ø¼­
	{2,1},{1,2},{-2,-1},{-1,-2},
	{-2,1},{-1,2},{2,-1},{1,-2}
};

int** visited, ret; 

typedef struct _queue {
	int row;
	int col;
	struct _queue* next;
}Queue;
Queue* front = NULL, * rear = NULL;

void Enqueue(int row, int col)
{
	Queue* tmp = (Queue*)malloc(sizeof(Queue));
	tmp->row = row;
	tmp->col = col;
	tmp->next = NULL;
	if (front == NULL)
	{
		front = tmp;
		rear = tmp;
	}
	else
	{
		rear->next = tmp;
		rear = tmp;
	}
}

void Dequeue(int* row, int* col)
{
	Queue* tmp = front;
	*row = tmp->row;
	*col = tmp->col;
	front = front->next;
	free(tmp);
}

void Clear_queue()
{
	for (; front != NULL;)
	{
		Queue* tmp = front;
		front = front->next;
		free(tmp);
	}
	front = NULL;
	rear = NULL;
}

int main()
{
	int T;
	scanf("%d", &T);
	int* output = (int*)malloc(sizeof(int) * T);

	for (int k = 0; k < T; k++) {
		int N;
		scanf("%d", &N);

		visited = (int**)malloc(sizeof(int*) * N);
		for (int i = 0; i < N; i++)
			visited[i] = (int*)malloc(sizeof(int) * N);

		int ret = -1, check = 0;
		// BFS
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				visited[i][j] = 0;

		Enqueue(0, 0);
		visited[0][0] = 1;
		
		int i, j;
		while (front != NULL)
		{
			Dequeue(&i, &j);

			if (i == N - 1 && j == N - 1)
			{
				ret = visited[N - 1][N - 1] - 1;
				check = 1;
				break;
			}

			for (int n = 0; n < 8; n++)
			{
				int nx = j + M[n].dx, ny = i + M[n].dy;
				if (nx >= 0 && nx < N && ny >= 0 && ny < N)
				{
					if (visited[ny][nx] == 0)
					{
						visited[ny][nx] = visited[i][j] + 1;
						Enqueue(ny, nx);
					}
				}
			}
		}
		if (check == 0)
			ret = -1;
		output[k] = ret;

		for (int i = 0; i < N; i++)
			free(visited[i]);
		free(visited);

		Clear_queue();
	}
	
	for (int k = 0; k < T; k++)
		printf("%d\n", output[k]);

	return 0;
}