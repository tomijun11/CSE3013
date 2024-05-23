#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
	int item;
	struct Node* next;
}node;
struct Node* top = NULL;
int size = 0;

int Empty()
{
	if (top == NULL)
		return 1;
	else
		return 0;
}

int Push(int item)
{
	node* tmp = (node*)malloc(sizeof(node));
	tmp->item = item;
	tmp->next = top;
	top = tmp;
	size++;
	return size;
}

int Pop()
{
	if (top == NULL)
		return -1;
	
	node* tmp = top;
	int item = tmp->item;
	top = top->next;
	free(tmp);
	size--;
	return item;
}

int Top()
{
	if (top == NULL)
		return -1;
	return top->item;
}

void Print()
{
	if (top == NULL)
		printf("-1");

	node* ptr = top;
	for (; ptr != NULL; ptr = ptr->next)
		printf("%d ", ptr->item);
}

int Swap()
{
	if (size < 2)
	{
		printf("-1");
	}
	else {
		node* tmp1 = top, *tmp2 = top->next;
		tmp1->next = tmp2->next;
		tmp2->next = tmp1;
		top = tmp2;
		Print();
	}
}


int main()
{
	while (1)
	{
		int N, num;
		scanf("%d", &N);
		
		switch (N)
		{
		case 0:
			printf("%d\n", Empty());
			break;
		case 1:
			scanf("%d", &num);
			printf("%d\n", Push(num));
			break;
		case 2:
			printf("%d\n", Pop());
			break;
		case 3:
			printf("%d\n", Top());
			break;
		case 4:
			Swap();
			printf("\n");
			break;
		case 5:
			Print();
			printf("\n");
			break;
		case 6: // Quit
			goto exit;
		}
	}
	exit:

	return 0;
}