#include <iostream>
#include "ofApp.h"
#include "WaterFall.h"
#include "Maze.h"
#include "Calc.h"

// ofApp.h ���������� link
#define Stage_Num 5
extern MData MArr[Stage_Num];
extern Node* Mtop;
extern vector<Line> Path;
extern Player Box;

void DrawMaze(int N) // Draw Maze (��Ʈ ���� DFS ������� ��Ʈ�� �����, ù ȣ��� ���� ȭ�� ũ�⸦ �������� �̷��� Orig, WOrig, WType�� �����)
{
	// �̷� ���� ���� : ȸ��, �β��� zoom�� 5��
	ofSetColor(128);
	ofSetLineWidth(Box.zoom * 5);
	double L = Box.L, T = 5 * Box.zoom, T2 = T / 2; // �ڵ� ����ȭ�� �ӽú���
	// �̷��� ���� �׸�
	TR_ofDrawLine(Box.baseX, Box.baseY, Box.baseX + L * (MArr[N].Col), Box.baseY); // �̷��� �� ����
	TR_ofDrawLine(Box.baseX, Box.baseY, Box.baseX, Box.baseY + L * (MArr[N].Row)); // �̷��� �� ������
	int i, j;
	for (i = 0; i < MArr[N].Row; i++) // �̷��� ��� ����� ���� �׸�
	{
		for (j = 0; j < MArr[N].Col; j++)
		{
			if (MArr[N].Maze[i][j].r == 1) // ������
				TR_ofDrawLine(Box.baseX + (j + 1) * L, Box.baseY + i * L, Box.baseX + (j + 1) * L, Box.baseY + (i + 1) * L);
			if (MArr[N].Maze[i][j].d == 1) // �Ʒ���
				TR_ofDrawLine(Box.baseX + j * L, Box.baseY + (i + 1) * L, Box.baseX + (j + 1) * L, Box.baseY + (i + 1) * L);
		}
	}

	if (MArr[N].Orig.size() == 0) // �ش� MArr[N]�� �̷θ� ó������ �׸� ��� �̷� �����ͷ� Orig, WOrig, WType�� ��� (���� ScrX, ScrY�� �����)
	{
		double X1 = Box.baseX + 0 * L, Y1 = Box.baseY + 0 * L, X2 = Box.baseX + MArr[N].Col * L, Y2 = Box.baseY + MArr[N].Row * L;
		int Row = MArr[N].Row, Col = MArr[N].Col;
		
		// �׵θ� (�ܰ� ��輱)
		// �÷��̾� �浹���� (Orig)�� �̷��� �ܰ��� 4���� �߰��� (�β��� 2T�� Block���·� ����)
		MArr[N].Orig.push_back(MakeBlock(X1, Y1 - T, X2, Y1 - T, X2, Y1 + T, X1, Y1 + T)); // ��
		MArr[N].Orig.push_back(MakeBlock(X2 - T, Y1, X2 + T, Y1, X2 + T, Y2, X2 - T, Y2)); // ��
		MArr[N].Orig.push_back(MakeBlock(X1, Y2 - T, X2, Y2 - T, X2, Y2 + T, X1, Y2 + T)); // ��
		MArr[N].Orig.push_back(MakeBlock(X1 - T, Y1, X1 + T, Y1, X1 + T, Y2, X1 - T, Y2)); // ��

		// WOrig�� ó�� (L >> T�� ������)
		for (i = 0; i < Col; i++) // ���� �ܰ��� ó��
		{
			double lx = 0, rx = 0, lx2 = 0, rx2 = 0; // ���� ĭ���� ���� �β��� ����Ͽ� �߰��� ������ ���� ��ǥ ������ (0 : ������, �ٸ��� : ������)
			if (i > 0 && MArr[N].Maze[0][i - 1].r == 1) // ������ �� ���� (��)
				lx = T;
			if (i < Col - 1 && MArr[N].Maze[0][i].r == 1) // ������ �� ���� (��)
				rx = -T;
			if (i > 0 && MArr[N].Maze[Row - 1][i - 1].r == 1) // ������ �� ���� (��)
				lx2 = T;
			if (i < Col - 1 && MArr[N].Maze[Row - 1][i].r == 1) // ������ �� ���� (��)
				rx2 = -T;
			MArr[N].WOrig.push_back(MakeLine(X1 + L * i + lx, Y1, X1 + L * (i + 1) + rx, Y1)); // ���� ���̷� ���μ� �߰�
			MArr[N].Wtype.push_back(1); 
			MArr[N].WOrig.push_back(MakeLine(X1 + L * i + lx2, Y2, X1 + L * (i + 1) + rx2, Y2)); // ���� ���̷� ���μ� �߰�
			MArr[N].Wtype.push_back(1); 
		}

		for (i = 0; i < Row; i++) // ���� �ܰ��� ó��
		{
			double uy = 0, dy = 0, uy2 = 0, dy2 = 0; // ���� ĭ���� ���� �β��� ����Ͽ� �߰��� ������ ���� ��ǥ ������ (0 : ������, �ٸ��� : ������)
			if (i > 0 && MArr[N].Maze[i - 1][0].d == 1) // ���ʿ� �� ���� (��)
				uy = T;
			if (i < Row - 1 && MArr[N].Maze[i][0].d == 1) // �Ʒ��� �� ���� (��)
				dy = -T;
			if (i > 0 && MArr[N].Maze[i - 1][Col - 1].d == 1) // ���ʿ� �� ���� (��)
				uy2 = T;
			if (i < Row - 1 && MArr[N].Maze[i][Col - 1].d == 1) // �Ʒ��� �� ���� (��)
				dy2 = -T;
			MArr[N].WOrig.push_back(MakeLine(X1, Y1 + L * i + uy, X1, Y1 + L * (i + 1) + dy)); // ���� ���̷� ���μ� �߰�
			MArr[N].Wtype.push_back(0);
			MArr[N].WOrig.push_back(MakeLine(X2, Y1 + L * i + uy2, X2, Y1 + L * (i + 1) + dy2)); // ���� ���̷� ���μ� �߰�
			MArr[N].Wtype.push_back(0);
		}

		// ���� �� (�̷��� �� ���� ��ȸ��), WOrig ���� Orig�� �����ϴ� 4���� ���е� �� ���� ��ġ�� �ʴ� ���и� �����Ͽ� �߰��� (���� ���� �������� ���� �̾��� ���е��� ��� ���ļ� �ϳ��� ����)
		for (i = 0; i < Row; i++)
		{
			for (j = 0; j < Col; j++)
			{
				if (j < Col - 1 && MArr[N].Maze[i][j].r == 1) // �̷��� ���� ���� �ִ� ��� (�ܰ��� ����)
				{
					double X = Box.baseX + (j + 1) * L, Y1 = Box.baseY + (i)*L, Y2 = Box.baseY + (i + 1) * L;
					MArr[N].Orig.push_back(MakeBlock(X - T, Y1, X + T, Y1, X + T, Y2, X - T, Y2)); // �������� ���α��̰� 2T�� Block���·� ����

					MArr[N].WOrig.push_back(MakeLine(X, Y1, X, Y2)); // ������ �߽ɿ� ���� ���μ� �߰�
					MArr[N].Wtype.push_back(5);

					int check1 = 1, check2 = 1, c1 = 0, c2 = 0; // ���� ��, �Ʒ� �β� ���μ� �߰����� (1:�߰�, 0:�߰�����), ���μ��� ���̸� �β���ŭ �߶����� ���� (��, �Ʒ�)
					if (i == 0) // �� ����
					{
						if (MArr[N].Maze[0][j].d == 1 || (j <= Col - 2 && MArr[N].Maze[0][j + 1].d == 1)) // �������� ���ʾƷ�((0,j).d), �Ǵ� ������ �Ʒ�((0,j+1).d))�� �ִ� ���
						{
							// ���μ��� �Ʒ����� �β�(2T)��ŭ ���̰�, �Ʒ��� ���� �β����� �߰����� �ʴ´�.
							check2 = 0;
							c2 = 1;
						}
						if (c2 == 1) // �Ʒ��� ���̸� ���� ���
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1, X - T, Y2 - T));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1, X + T, Y2 - T));
						}
						else // ������ ���� ���
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1, X - T, Y2 + T));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1, X + T, Y2 + T));
						}
						MArr[N].Wtype.push_back(0); // �߰��Ǵ� �� ���� ��� ���μ� (type = 0)
						MArr[N].Wtype.push_back(0);
					}
					else if (i == Row - 1) // �� �Ʒ���
					{
						if (MArr[N].Maze[Row - 2][j].d == 1 || (j <= Col - 2 && MArr[N].Maze[Row - 2][j + 1].d == 1)) // �������� ������((row-2,j).d), �Ǵ� ������ ��((row-2,j+1).d))�� �ִ� ���
						{
							// ���μ��� ������ �β�(2T)��ŭ ���̰�, ���� ���� �β����� �߰����� �ʴ´�.
							check1 = 0;
							c1 = 1;
						}
						if (c1 == 1) // ���� ���̸� ���� ���
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1 + T, X - T, Y2));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1 + T, X + T, Y2));
						}
						else  // ������ ���� ���
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1 - T, X - T, Y2));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1 - T, X + T, Y2));
						}
						MArr[N].Wtype.push_back(0); // �߰��Ǵ� �� ���� ��� ���μ� (type = 0)
						MArr[N].Wtype.push_back(0);
					}
					else 
					{
						if (MArr[N].Maze[i - 1][j].d == 1 || (j <= Col - 2 && MArr[N].Maze[i - 1][j + 1].d == 1)) // �������� ������((i-1,j).d), �Ǵ� ������ ��((i-1,j+1).d))�� �ִ� ���
						{
							// ���μ��� ������ �β�(2T)��ŭ ���̰�, ���� ���� �β����� �߰����� �ʴ´�.
							check1 = 0;
							c1 = 1;
						}
						if (MArr[N].Maze[i][j].d == 1 || (j <= Col - 2 && MArr[N].Maze[i][j + 1].d == 1)) // �������� ���ʾƷ�((i,j).d), �Ǵ� ������ �Ʒ�((i,j+1).d))�� �ִ� ���
						{
							// ���μ��� �Ʒ����� �β�(2T)��ŭ ���̰�, �Ʒ��� ���� �β����� �߰����� �ʴ´�.
							check2 = 0;
							c2 = 1;
						}

						// ���� ���̿� �°� ������ �߰���
						if (c1 == 1 && c2 == 1)
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1 + T, X - T, Y2 - T));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1 + T, X + T, Y2 - T));
						}
						else if (c1 == 1)
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1 + T, X - T, Y2 + T));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1 + T, X + T, Y2 + T));
						}
						else if (c2 == 1)
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1 - T, X - T, Y2 - T));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1 - T, X + T, Y2 - T));
						}
						else
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1 - T, X - T, Y2 + T));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1 - T, X + T, Y2 + T));
						}
						MArr[N].Wtype.push_back(0); // �߰��Ǵ� �� ���� ��� ���μ� (type = 0)
						MArr[N].Wtype.push_back(0);
					}

					if (check1 == 1 && (i >= 1 && MArr[N].Maze[i - 1][j].r == 0)) // �����ʺ��� ����((i-1,j).r)�� ���� ��� ���� ���� �β����� �߰��� 
					{
						MArr[N].WOrig.push_back(MakeLine(X - T, Y1 - T, X + T, Y1 - T));
						MArr[N].Wtype.push_back(2); // �߰��Ǵ� ���� ���� �β��� (type = 2)
					}

					if (check2 == 1 && (i <= Row - 2 && MArr[N].Maze[i + 1][j].r == 0)) // �����ʺ��� �Ʒ���((i+1,j).r)�� ���� ��� �Ʒ��� ���� �β����� �߰��� 
					{
						MArr[N].WOrig.push_back(MakeLine(X - T, Y2 + T, X + T, Y2 + T));
						MArr[N].Wtype.push_back(2); // �߰��Ǵ� ���� ���� �β��� (type = 2)
					}
				}
				if (i < Row - 1 && MArr[N].Maze[i][j].d == 1) // �Ʒ��� ���� �ִ� ��� (�ܰ��� ����)
				{
					double X1 = Box.baseX + (j)*L, X2 = Box.baseX + (j + 1) * L, Y = Box.baseY + (i + 1) * L;
					MArr[N].Orig.push_back(MakeBlock(X1, Y - T, X2, Y - T, X2, Y + T, X1, Y + T)); // ���� ���̰� 2T�� Block ���·� ���� 

					MArr[N].WOrig.push_back(MakeLine(X1, Y, X2, Y)); // �Ʒ��� ���ο� ���� ���μ� �߰�
					MArr[N].Wtype.push_back(4);

					int check1 = 1, check2 = 1; // ���� ����, ������ �β� ���μ� �߰����� (1:�߰�, 0:�߰�����)
					double lux = -T, ldx = -T, rux = T, rdx = T; // �β���ŭ ���⿡ ���� �߶󳾶� ����� ����
					if (j == 0) // �� ������
					{
						if (MArr[N].Maze[i][0].r == 1) // �Ʒ����� ������ ����((i,0).r)�� �ִ� ���
						{
							// ���� ���μ��� �����ʿ��� �β�(2T)��ŭ ���̰�, ������ ���� �β����� �߰����� �ʴ´�.
							check2 = 0;
							rux = -T;
						}
						if ((i <= Row - 2 && MArr[N].Maze[i + 1][0].r == 1)) // �Ʒ����� ������ �Ʒ���((i+1,0).r)�� �ִ� ���
						{
							// �Ʒ��� ���μ��� �����ʿ��� �β�(2T)��ŭ ���̰�, ������ ���� �β����� �߰����� �ʴ´�.
							check2 = 0;
							rdx = -T;
						}
						MArr[N].WOrig.push_back(MakeLine(X1, Y - T, X2 + rux, Y - T)); // ���� ���̿� �°� ������ �߰���
						MArr[N].WOrig.push_back(MakeLine(X1, Y + T, X2 + rdx, Y + T));
						MArr[N].Wtype.push_back(1); // �߰��Ǵ� �� ���� ��� ���μ� (type = 1)
						MArr[N].Wtype.push_back(1);
					}
					else if (j == Col - 1) // �� ��������
					{
						if (MArr[N].Maze[i][Col - 2].r == 1) // �Ʒ����� ���� ����((i,col-2).r)�� �ִ� ���
						{
							// ���� ���μ��� ���ʿ��� �β�(2T)��ŭ ���̰�, ���� ���� �β����� �߰����� �ʴ´�.
							check1 = 0;
							lux = T;
						}
						if ((i <= Row - 2 && MArr[N].Maze[i + 1][Col - 2].r == 1)) // �Ʒ����� ������ �Ʒ���((i+1,col-2).r)�� �ִ� ���
						{
							// �Ʒ��� ���μ��� ���ʿ��� �β�(2T)��ŭ ���̰�, ���� ���� �β����� �߰����� �ʴ´�.
							check1 = 0;
							ldx = T;
						}
						MArr[N].WOrig.push_back(MakeLine(X1 + lux, Y - T, X2, Y - T)); // ���� ���̿� �°� ������ �߰���
						MArr[N].WOrig.push_back(MakeLine(X1 + ldx, Y + T, X2, Y + T));
						MArr[N].Wtype.push_back(1); // �߰��Ǵ� �� ���� ��� ���μ� (type = 1)
						MArr[N].Wtype.push_back(1);
					}
					else
					{
						if (MArr[N].Maze[i][j].r == 1)  // �Ʒ����� ������ ����((i,j).r)�� �ִ� ���
						{
							// ���� ���μ��� �����ʿ��� �β�(2T)��ŭ ���̰�, ������ ���� �β����� �߰����� �ʴ´�.
							check2 = 0;
							rux = -T;
						}
						if ((i <= Row - 2 && MArr[N].Maze[i + 1][j].r == 1))  // �Ʒ����� ������ �Ʒ���((i+1,j).r)�� �ִ� ���
						{
							// �Ʒ��� ���μ��� �����ʿ��� �β�(2T)��ŭ ���̰�, ������ ���� �β����� �߰����� �ʴ´�.
							check2 = 0;
							rdx = -T;
						}
						if (MArr[N].Maze[i][j - 1].r == 1) // �Ʒ����� ���� ����((i,j-1).r)�� �ִ� ���
						{
							// ���� ���μ��� ���ʿ��� �β�(2T)��ŭ ���̰�, ���� ���� �β����� �߰����� �ʴ´�.
							check1 = 0;
							lux = T;
						}
						if ((i <= Row - 2 && MArr[N].Maze[i + 1][j - 1].r == 1)) // �Ʒ����� ������ �Ʒ���((i+1,j-1).r)�� �ִ� ���
						{
							// �Ʒ��� ���μ��� ���ʿ��� �β�(2T)��ŭ ���̰�, ���� ���� �β����� �߰����� �ʴ´�.
							check1 = 0;
							ldx = T;
						}
						MArr[N].WOrig.push_back(MakeLine(X1 + lux, Y - T, X2 + rux, Y - T)); // ���� ���̿� �°� ������ �߰���
						MArr[N].WOrig.push_back(MakeLine(X1 + ldx, Y + T, X2 + rdx, Y + T));
						MArr[N].Wtype.push_back(1); // �߰��Ǵ� �� ���� ��� ���μ� (type = 1)
						MArr[N].Wtype.push_back(1);
					}

					if (j >= 1 && MArr[N].Maze[i][j - 1].d == 0) // �Ʒ����� ���ʺ�((i,j-1).d)�� ���� ��� ���� ���� �β����� �߰��� 
					{
						MArr[N].WOrig.push_back(MakeLine(X1 - T, Y - T, X1 - T, Y + T));
						if (check1 == 0) // true�� �ش� ��ġ(����)�� ���κ��� ������ -> �β����� �ƴ� ���μ��� �߰��Ѵ�.
							MArr[N].Wtype.push_back(0); // ���μ� �߰� (type = 0)
						else
							MArr[N].Wtype.push_back(2); // �β��� �߰� (type = 2)
					}
					if (i <= Col - 2 && MArr[N].Maze[i][j + 1].d == 0) // �Ʒ����� �����ʺ�((i,j+1).d)�� ���� ��� ������ ���� �β����� �߰��� 
					{
						MArr[N].WOrig.push_back(MakeLine(X2 + T, Y - T, X2 + T, Y + T));
						if (check2 == 0) // true�� �ش� ��ġ(������)�� ���κ��� ������ -> �β����� �ƴ� ���μ��� �߰��Ѵ�.
							MArr[N].Wtype.push_back(0); // ���μ� �߰� (type = 0)
						else
							MArr[N].Wtype.push_back(2); // �β��� �߰� (type = 2)
					}
				}
			}
		}

		int count = 0, total = MArr[N].WOrig.size(); // WOrig�� �� ����
		vector<Line> &larr = MArr[N].WOrig; // �ڵ� ����ȭ�� ������ ���
		vector<int> &type = MArr[N].Wtype;

		// WOrig�� �߰��ߴ� ���е��� ���� �������� �̾����� ���� ���� �����Ͽ� �߰��ϴ� �ܰ�
		for (i = 0; i < total; i++) // WOrig ��ü ��ȸ
		{
			if (type[i] == 1) // ����
			{
				while (1) {
					int check = 0;
					for (j = 0; j < total; j++) // ���� ��ȸ
					{
						if (i != j && type[j] == 1) // ���� ���õ� ����(i)�� �ٸ� �����̸鼭 Ÿ���� ����(1) ������ ���
						{
							if (larr[i].y1 == larr[j].y1) // Y��ǥ�� ���� ������
							{
								// j�� x��ǥ�� i�� ���� ����(����� ����)�� �ְų� 
								if (larr[i].x1 <= larr[j].x1 && larr[i].x2 >= larr[j].x1)
								{
									larr[i] = MakeLine(larr[i].x1, larr[i].y1, larr[j].x2, larr[i].y1);
									type[j] = 3;
									check = 1;
								}
								// i�� x��ǥ�� j�� ���� ����(����� ����)�� ������ 
								else if (larr[i].x1 >= larr[j].x1 && larr[i].x1 <= larr[j].x2)
								{
									// �ϳ��� ������ ����� ���� j�� ���� ������ ������ (i���� type�� 3���� ������ ���̻� ������ �Ͼ�� �ʰ� ó����)
									larr[i] = MakeLine(larr[j].x1, larr[i].y1, larr[i].x2, larr[i].y1);
									type[j] = 3;
									check = 1;
								}
							}
						}
					}
					if (check == 0) // ���̻� ���� ������ ���������� �ݺ� -> break
						break;
				}
			}
			else if (type[i] == 0) // ����
			{
				while (1)
				{
					int check = 0;
					for (j = 0; j < total; j++)  // ���� ��ȸ
					{
						if (i != j && type[j] == 0) // ���� ���õ� ����(i)�� �ٸ� �����̸鼭 Ÿ���� ����(0) ������ ���
						{
							if (larr[i].x1 == larr[j].x1) // X��ǥ�� ���� ������
							{
								// j�� y��ǥ�� i�� ���� ����(����� ����)�� �ְų�  
								if (larr[i].y1 <= larr[j].y1 && larr[i].y2 >= larr[j].y1)
								{
									larr[j] = MakeLine(larr[i].x1, larr[i].y1, larr[i].x1, larr[j].y2);
									type[i] = 3;
								}
								// i�� y��ǥ�� j�� ���� ����(����� ����)�� ������ 
								else if (larr[i].y1 >= larr[j].y1 && larr[i].y1 <= larr[j].y2)
								{
									// �ϳ��� ������ ����� ���� j�� ���� ������ ������ (i���� type�� 3���� ������ ���̻� ������ �Ͼ�� �ʰ� ó����)
									larr[j] = MakeLine(larr[i].x1, larr[j].y1, larr[i].x1, larr[i].y2);
									type[i] = 3;
								}
							}
						}
					}
					if (check == 0) // 1ȸ ��ȯ�� type�� 0�̰�, ���� i�� �̾��� ��� ���е��� ������ �Ϸ�� -> break
						break;
				}
			}

			else if (type[i] == 5) // ���� (����)
			{
				while (1)
				{
					int check = 0;
					for (j = 0; j < total; j++) // ���� ��ȸ
					{
						if (i != j && type[j] == 5) // ���� ���õ� ����(i)�� �ٸ� �����̸鼭 Ÿ���� ����(5) ������ ���
						{
							if (larr[i].x1 == larr[j].x1) // X��ǥ�� ���� ������
							{
								// j�� y��ǥ�� i�� ���� ����(����� ����)�� �ְų�  
								if (larr[i].y1 <= larr[j].y1 && larr[i].y2 >= larr[j].y1)
								{
									larr[j] = MakeLine(larr[i].x1, larr[i].y1, larr[i].x1, larr[j].y2);
									type[i] = 3;
								}
								// i�� y��ǥ�� j�� ���� ����(����� ����)�� ������ 
								else if (larr[i].y1 >= larr[j].y1 && larr[i].y1 <= larr[j].y2)
								{
									// �ϳ��� ������ ����� ���� j�� ���� ������ ������ (i���� type�� 3���� ������ ���̻� ������ �Ͼ�� �ʰ� ó����)
									larr[j] = MakeLine(larr[i].x1, larr[j].y1, larr[i].x1, larr[i].y2); 
									type[i] = 3;
								}
							}
						}
					}
					if (check == 0) // 1ȸ ��ȯ�� type�� 5��, ���� i�� �̾��� ��� ���е��� ������ �Ϸ�� -> break
						break;
				}
			}

			if (type[i] == 4) // ���� (����)
			{
				while (1) {
					int check = 0;
					for (j = 0; j < total; j++) // ���� ��ȸ
					{
						if (i != j && type[j] == 4) // ���� ���õ� ����(i)�� �ٸ� �����̸鼭 Ÿ���� ����(4) ������ ���
						{
							if (larr[i].y1 == larr[j].y1) // Y��ǥ�� ���� ������ 
							{
								// j�� x��ǥ�� i�� ���� ����(����� ����)�� �ְų� 
								if (larr[i].x1 <= larr[j].x1 && larr[i].x2 >= larr[j].x1)
								{
									larr[i] = MakeLine(larr[i].x1, larr[i].y1, larr[j].x2, larr[i].y1);
									type[j] = 3;
									check = 1;
								}
								// i�� x��ǥ�� j�� ���� ����(����� ����)�� ������ 
								else if (larr[i].x1 >= larr[j].x1 && larr[i].x1 <= larr[j].x2)
								{
									// �ϳ��� ������ ����� ���� j�� ���� ������ ������ (i���� type�� 3���� ������ ���̻� ������ �Ͼ�� �ʰ� ó����)
									larr[i] = MakeLine(larr[j].x1, larr[i].y1, larr[i].x2, larr[i].y1);
									type[j] = 3;
									check = 1;
								}
							}
						}
					}
					if (check == 0)  // ���̻� ���� ������ ���������� �ݺ� -> break
						break;
				}
			}

		}


		for (i = 0; i < larr.size(); i++) // Wall�� WTest�� size ���
			if (type[i] != 3)
				count++;
		MArr[N].Trans.resize(MArr[N].Orig.size()); // Trans�� Orig�� ũ��� ���� -> ������ size ������
		// Wall�� WTest�� WOrig���� Type�� 3�� ���� ������ ��ҵ��� ���� -> ������ size�� ������
		MArr[N].Wall.resize(count);
		MArr[N].WTest.resize(count);

	}

	if (Box.hintTimer > 0 && ofGetElapsedTimef() - Box.hintStartTime < 10.0) // ��Ʈ�� ����߰�, ��Ʈ�ð��� �������
	{
		Box.hintTimer = ofGetElapsedTimef() - Box.hintStartTime;
		DFS(N); // DFS �������� ��� ��� (Path)
		dfsdraw(); // ��θ� �׸�
	}
}

//////////////////

int CheckMaze(int N) // �̷��� ��(�÷��̾�-�� �浹����)�� �浹�ϴ��� üũ��
{
	int i, j, ret = 0;
	for (i = 0; i < MArr[N].Trans.size(); i++) // Trans�� ��ȸ�ϸ鼭 �̷��� ��� �浹���� ���� ���������� üũ��
	{
		if (CheckCollide(MArr[N].Trans[i]))
			ret = 1; // �ϳ��� ������ �浹�Ѵٷ� ������
	}
	return ret;
}

int CheckCircle(double X, double Y, double R)// Circle���ο� �÷��̾ �����ϴ��� �˻��� (������� �ٻ���)
{
	Block Temp; // ������� �ٻ�
	Temp.P[0] = { X - R,Y - R };
	Temp.P[1] = { X + R,Y - R };
	Temp.P[2] = { X + R,Y + R };
	Temp.P[3] = { X - R,Y + R };
	return CheckCollide(Temp); // CheckCollide�� Ȱ����
}

int CheckCollide(Block Target)  // Block���ο� �÷��̾ �����ϴ��� �˻��� (��ȯ�� ��ǥ�� ����������)
{
	int count = 0;
	for (int i = 0; i < 4; i++) { 
		int j = (i + 1) % 4; // i, j�� �簢���� �� ���� �����ϰԵ�
		if ((Target.P[i].Y > Box.TLoc.Y) != (Target.P[j].Y > Box.TLoc.Y)) // �ش� ���� �÷��̾� ��ǥ�� ���ʿ� �ְų� �Ʒ��ʿ� ������ �Ѿ
		{
			if (Target.P[j].Y == Target.P[i].Y) // ������ ����� ó��
			{
				if (Target.P[i].Y == Box.TLoc.Y) // Y�� ������ -> count 1 ����
					count++;
			}
			else
			{
				// �������� ������ ��� �ش� ���� ���� ������ ������ ã�Ƽ� XLoc�� ����
				double XLoc = (Target.P[j].X - Target.P[i].X) * (Box.TLoc.Y - Target.P[i].Y) / (Target.P[j].Y - Target.P[i].Y) + Target.P[i].X;
				if (Box.TLoc.X < XLoc) // �����ʿ��� ������ -> Count 1 ����
					count++;
			}
		}
	}
	return count % 2; // ������ ���� Ȧ���� = ����, ¦���� = �ܺ�
}

//////////////////

void DFS(int N) // DFS�� �����ϴ� �Լ�
{
	int i, j;

	// �̷��� visited �ʱ�ȭ
	for (i = 0; i < MArr[N].Row; i++)
		for (j = 0; j < MArr[N].Col; j++)
			MArr[N].Maze[i][j].visited = 0;

	ClearStack(); // ���ð� Path�� �ʱ�ȭ��
	Path.clear();

	int sx, sy; // �÷��̾��� �ε��� ��ġ (DFS�� Ž�� �������� ��)
	calc_Loc(&sx, &sy, N); // �÷��̾��� ���� �� ��ġ�� ���� (�ε��� ��ġ)
	Push(sy, sx); // ������ ��ġ
	MArr[N].Maze[sy][sx].visited = 1;
	while (Mtop != NULL) // iterative DFS 
	{
		i = Mtop->row;
		j = Mtop->col;

		if (i == MArr[N].Row - 1 && j == MArr[N].Col - 1) // ������ ��ġ
			break;

		if (MArr[N].Maze[i][j].r == 0 && MArr[N].Maze[i][j + 1].visited == 0) // r Ž��
		{
			MArr[N].Maze[i][j + 1].visited = 1;
			Push(i, j + 1);
		}
		else if (MArr[N].Maze[i][j].d == 0 && MArr[N].Maze[i + 1][j].visited == 0) // d Ž��
		{
			MArr[N].Maze[i + 1][j].visited = 1;
			Push(i + 1, j);
		}
		else if (j > 0 && MArr[N].Maze[i][j - 1].r == 0 && MArr[N].Maze[i][j - 1].visited == 0) // l Ž��
		{
			MArr[N].Maze[i][j - 1].visited = 1;
			Push(i, j - 1);
		}
		else if (i > 0 && MArr[N].Maze[i - 1][j].d == 0 && MArr[N].Maze[i - 1][j].visited == 0) // u Ž��
		{
			MArr[N].Maze[i - 1][j].visited = 1;
			Push(i - 1, j);
		}
		else
			Pop(&i, &j); // ���̻� Ž���� ��� ���� -> Pop
	}

	int ni, nj; // ���� ��ġ
	Pop(&i, &j); // i, j�� ������ ��ġ ����
	for (; Mtop != NULL;) // DFS�� ����� ���ÿ��� ���������� ������������ ��ΰ� �����
	{
		ni = Mtop->row; // ���� ��ġ ����
		nj = Mtop->col;
		if (ni == i) // r �Ǵ� l
		{
			if (nj > j) // l
			{
				Line ltemp = { Box.baseX + Box.L * (nj + 0.5), Box.baseY + Box.L * (ni + 0.5), Box.baseX + Box.L * (nj - 0.5), Box.baseY + Box.L * (ni + 0.5) };
				Path.push_back(ltemp);
			}
			else // r
			{
				Line ltemp = { Box.baseX + Box.L * (nj + 0.5), Box.baseY + Box.L * (ni + 0.5), Box.baseX + Box.L * (nj + 1.5), Box.baseY + Box.L * (ni + 0.5) };
				Path.push_back(ltemp);
			}
		}
		else if (nj == j) // d �Ǵ� u
		{
			if (ni > i) // u
			{
				Line ltemp = { Box.baseX + Box.L * (nj + 0.5), Box.baseY + Box.L * (ni + 0.5), Box.baseX + Box.L * (nj + 0.5), Box.baseY + Box.L * (ni - 0.5) };
				Path.push_back(ltemp);
			}
			else // d
			{
				Line ltemp = { Box.baseX + Box.L * (nj + 0.5), Box.baseY + Box.L * (ni + 0.5), Box.baseX + Box.L * (nj + 0.5), Box.baseY + Box.L * (ni + 1.5) };
				Path.push_back(ltemp);
			}
		}

		Pop(&i, &j); // i, j ����
	}
}

void dfsdraw() // DFS�� ���� ������������ ��θ� �����
{
	int i;
	ofSetColor(255, 128, 0); // ��Ȳ������ ��θ� �׸�
	ofSetLineWidth(Box.zoom * 5);
	for (i = 0; i < Path.size(); i++) // Path�� ��ȸ�ϸ鼭 ���� Line�� ��ȯ�� ��ǥ�� �׸�
	{
		Line ltemp = Path.at(i);
		TR_ofDrawLine(ltemp.x1, ltemp.y1, ltemp.x2, ltemp.y2);
	}
}

//////////////////

void Push(int row, int col) // DFS�� ���� Push
{
	Node* temp = (Node*)malloc(sizeof(Node));
	temp->row = row;
	temp->col = col;
	temp->next = NULL;

	if (Mtop == NULL)
	{
		Mtop = temp;
	}
	else
	{
		temp->next = Mtop;
		Mtop = temp;
	}
}

void Pop(int* row, int* col) // DFS�� ���� Pop
{
	if (Mtop == NULL) // ���� ��� ������ ����
	{
		*row = -1;
		return;
	}

	Node* temp = Mtop;
	*row = temp->row;
	*col = temp->col;

	Mtop = temp->next;
	free(temp);
}

void ClearStack() // DFS�� ������ �ʱ�ȭ�ϴ� �Լ�
{
	for (; Mtop != NULL;) // ������ ������ ��ȸ�ϸ鼭 �Ҵ�������
	{
		Node* temp = Mtop;
		Mtop = Mtop->next;
		free(temp);
	}
	Mtop = NULL;
}
