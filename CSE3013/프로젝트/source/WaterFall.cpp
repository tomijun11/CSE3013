#include <iostream>
#include "ofApp.h"
#include "WaterFall.h"
#include "Maze.h"
#include "Matrix.h"
#include "Calc.h"

// ofApp.h ���������� link
#define Stage_Num 5
extern MData MArr[Stage_Num];
extern Player Box;
extern double Matrix[3][3];
extern double Inverse[3][3];

void DrawWater(int N)  // ���ٱ� �� ��� ��� ����� �׸�
{
	for (int k = 0; k < MArr[N].Hole.size(); k++) // ���ٱ� ������ �迭 ��ȸ
	{
		ofSetLineWidth(MArr[N].Hole[k].t); // �׸� ���� ���⸦ ���ٱ��� �β��� ����
		if (MArr[N].Hole[k].type == 0) // ���ٱ� ���� -> �Ķ��� ��
		{
			ofSetColor(0, 0, 255);
			TR_ofDrawCircle(MArr[N].Hole[k].Dot.X, MArr[N].Hole[k].Dot.Y, 10);
			ofSetColor(0, 255, 255); // ���ٱ� ���� -> cyan
		}
		else // ��� ���� -> ��ο� ������ ��
		{
			ofSetColor(128, 0, 0);
			TR_ofDrawCircle(MArr[N].Hole[k].Dot.X, MArr[N].Hole[k].Dot.Y, 10);
			ofSetColor(255, 0, 0); // ��� ���� -> red
		}
		if (MArr[N].Hole[k].run == 1) // ���ٱⰡ �������� ���
		{
			Water_calcpath(N, MArr[N].Hole[k]); // ���ٱ��� ��� ��� (Path�� wblock ���)
			vector<Point>& Path = MArr[N].Hole[k].Path; // Path ��ȸ (���ٱ� ���)
			for (int i = 0; i < MArr[N].Hole[k].size - 1; i++)
			{
				ofDrawLine(Path[i].X, Path[i].Y, Path[i + 1].X, Path[i + 1].Y); // ���ٱ⸦ ������ �׸�
			}
		}
	}
}

//////////////////

void Water_addpath(Water &Wobj, double X, double Y, int index) // Path�� ���ٱ��� ���(��)�� �߰���
{
	if (index >= Wobj.Path.size()) // index(������ġ)�� Path�� size�� ������ ���
	{
		Wobj.Path.push_back({ X, Y }); // Path�� �� ĭ �÷��� �� �ڿ� ������
	}
	else
	{
		Wobj.Path[index] = { X,Y }; // Path�� ���� �������� ���� ��� �ش� ��ġ�� ���� ������
	}
}

void Water_addpath(Water &Wobj, Point P, int index) // �ٷ� �� �Լ��� overloading ����, X,Y��� Point�� ����
{
	if (index >= Wobj.Path.size())
	{
		Wobj.Path.push_back(P);
	}
	else
	{
		Wobj.Path[index] = P;
	}
}

void Water_addblock(Water& Wobj, Block B, int index) // wblock�� �÷��̾�-���ٱ� �浹���� Block�� �߰���
{
	if (index >= Wobj.wblock.size()) // index(������ġ)�� wblock�� size�� ������ ���
	{
		Wobj.wblock.push_back(B); // wblock�� �� ĭ �÷��� �� �ڿ� ������
	}
	else
	{
		Wobj.wblock[index] = B; // wblock�� ���� �������� ���� ��� �ش� ��ġ�� ���� ������
	}
}

//////////////////

void Water_calcpath(int N, Water &Wobj) // ���ٱ��� ��θ� ����ϴ� �Լ�
{
	int index = 0;
	Point TDot = Matrix_Trans(Matrix, Wobj.Dot); // ��ȯ�� ���ٱ��� ������ ��ǥ�� ����
	Water_addpath(Wobj, TDot, index++); // �������� ��ο� �߰�
	double prevY = -1, prevX = -1, startflag = 0, curY = TDot.Y, curX = TDot.X; // ���� ���� Y, X��ǥ, ���Ž������ flag, ���� X, Y��ǥ
	int mini, count = 0, pi = -1; // CurrentY�� �ּ��� i��, �ߺ��� ī��Ʈ, ������ ������ i��
	for (int l = 0; l < 200; l++) // �ִ� 200�� �ݺ� (���ѷ��� ����)
	{
		double minY = 100000; // ȭ�� Y��ǥ�� 10������ ����
		int i, j, check = 0; // ���� ������ ����(i,j), �ּҰ� ���ÿ���
		for (i = 0; i < MArr[N].Wall.size(); i++) // �̷��� ���ٱ� ���� �� �迭�� ��ȸ��
		{
			Line Cur = MArr[N].Wall[i]; // ���� ���õ� ��
			if (curY > Cur.y1  && curY > Cur.y2) continue; // ���� �������� �� ���� ������ġ���� ���� �ְų�
			if ((curX > Cur.x1) && (curX > Cur.x2)) continue; // ���ʿ� �ְų�
			if ((curX < Cur.x1) && (curX < Cur.x2)) continue; // �����ʿ� �ְų�
			// Slope ���
			if (Cur.x1 == Cur.x2) continue; // ���η� ������������ ��ŵ��

			double slope = (double)(Cur.y2 - Cur.y1) / (Cur.x2 - Cur.x1); // ���� ���� ���
			double currentY = slope * (curX - Cur.x1) + Cur.y1; // ���� ��ġ���� ���ٱⰡ �������� ���� �������� Y��ǥ(currentY)�� �����

			if (currentY < curY) // ���� ��ǥ�� ���� Y���� ���� ������ ��ŵ
				continue;

			if (MArr[N].WTest[i] <= 2 && startflag == 1) // ���� ������ �ܺμ� (����,����,�β�)�̰�, ������ġ�� �������� �ƴѰ��
			{
				if (curX == prevX && currentY == prevY) // ���� ������ġ�� ������ġ�� ���� ���
				{
					if (Cur.y1 >= Cur.y2) // ���� ���� Ÿ�� �帣�ٰ� ���������� �ȶ������� ������ ���μ����� �ٷ� ���󰡴� ��츦 ������
					{
						if (prevY >= Cur.y2) // ������ġ���� �� �������� ������ ��ŵ
							continue;
					}
					else
					{
						if (prevY >= Cur.y1) // ������ġ���� �� �������� ������ ��ŵ 
							continue;
					}
				}
			}

			if (pi != i && currentY <= minY) // ������ ������ ���� �ٸ���, ���� ������ġ�� minY���� ���� �ִ� ���
			{
				// �ּҰ� ����
				minY = currentY; 
				mini = i;
				check = 1; // �ּҰ� ��� ���� üũ
			}
		}
		if (check == 0 || (curY < prevY && startflag == 1)) // �ּҰ� ��� ������ ���ų� (�Ʒ��� �ܺμ��� ����), ������ġ�� ���� ��ġ���� ���� �ö󰡴� ��� ����߰��� �ߴ���
			break;


		Line Cur = MArr[N].Wall[mini]; // Cur�� ���õ� ���� ������
		// Path Added
		Water_addpath(Wobj, curX, minY, index++); // �� ���� �������� ��ο� �߰�
		if (MArr[N].WTest[mini] >= 4) // ���� ������ ���μ� (���� ����, ���� ���μ�)�� ��� �ߴ�
			break;

		// ���� ����
		if (Cur.y1 >= Cur.y2)
		{
			curX = Cur.x1;
			curY = Cur.y1;
		}
		else
		{
			curX = Cur.x2;
			curY = Cur.y2;
		}
		Water_addpath(Wobj, curX, curY, index++); // ���� ������ �� �� �Ʒ��� �ִ� ���� ��ο� �߰�

		if (curY == prevY && startflag == 1) // ������ġ�� �������� �ƴϰ�, �������� �������� Y��ǥ�� �������
		{
			if (count >= 1) // 2ȸ�̻� �ݺ��� ���� (v�� �� ������ ���̻� ����X)
				break;
			else
				count++;
		}
		else 
			count = 0; // Ż��� ī��Ʈ Ƚ���� �ʱ�ȭ��

		prevY = curY; // ���� ��ǥ�� ���� ��ǥ�� �����ϰ� ���� ������ �ѱ�
		prevX = curX;
		pi = mini;
		startflag = 1; 
	}
	Wobj.size = index; // size�� Path�� ����� ���� ������ ������

	// Path�� ����� ��θ� ����Ͽ� wblock�� �����
	double x1, x2, x3, x4, y1, y2, y3, y4, t = Wobj.t / 2;
	for (int i = 0; i < Wobj.size - 1; i++)
	{
		// Path���� �� ���� ������ �ϳ��� Block�� ����
		Point P1 = Wobj.Path[i];
		Point P2 = Wobj.Path[i + 1];
		if (P1.Y == P2.Y) // ������ ���簢��
		{
			x1 = P1.X; y1 = P1.Y - t;
			x2 = P1.X; y2 = P1.Y + t;
			x3 = P2.X; y3 = P2.Y - t;
			x4 = P2.X; y4 = P2.Y + t;
		}
		else // ���Ⱑ �ִ� ���簢�� (��ǥ �������� ����)
		{
			double slope = -(P2.X - P1.X) / (P2.Y - P1.Y), C = t / sqrt(1 + slope * slope);
			x1 = P1.X - C; y1 = slope * (x1 - P1.X) + P1.Y;
			x2 = P1.X + C; y2 = slope * (x2 - P1.X) + P1.Y;
			x3 = P2.X - C; y3 = slope * (x3 - P2.X) + P2.Y;
			x4 = P2.X + C; y4 = slope * (x4 - P2.X) + P2.Y;
		}
		Water_addblock(Wobj, MakeBlock(x1, y1, x2, y2, x4, y4, x3, y3), i); // �ϼ��� Block�� wblock�� �߰���
	}
}
