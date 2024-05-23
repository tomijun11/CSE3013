#include <iostream>
#include "ofApp.h"
#include "WaterFall.h"
#include "Maze.h"
#include "Calc.h"

// ofApp.h 전역변수들 link
#define Stage_Num 5
extern MData MArr[Stage_Num];
extern Node* Mtop;
extern vector<Line> Path;
extern Player Box;

void DrawMaze(int N) // Draw Maze (힌트 사용시 DFS 사용으로 힌트도 출력함, 첫 호출시 현재 화면 크기를 기준으로 미로의 Orig, WOrig, WType을 계산함)
{
	// 미로 벽의 색상 : 회색, 두께는 zoom의 5배
	ofSetColor(128);
	ofSetLineWidth(Box.zoom * 5);
	double L = Box.L, T = 5 * Box.zoom, T2 = T / 2; // 코드 간소화용 임시변수
	// 미로의 벽을 그림
	TR_ofDrawLine(Box.baseX, Box.baseY, Box.baseX + L * (MArr[N].Col), Box.baseY); // 미로의 맨 윗줄
	TR_ofDrawLine(Box.baseX, Box.baseY, Box.baseX, Box.baseY + L * (MArr[N].Row)); // 미로의 맨 왼쪽줄
	int i, j;
	for (i = 0; i < MArr[N].Row; i++) // 미로의 모든 방들의 벽을 그림
	{
		for (j = 0; j < MArr[N].Col; j++)
		{
			if (MArr[N].Maze[i][j].r == 1) // 우측벽
				TR_ofDrawLine(Box.baseX + (j + 1) * L, Box.baseY + i * L, Box.baseX + (j + 1) * L, Box.baseY + (i + 1) * L);
			if (MArr[N].Maze[i][j].d == 1) // 아래벽
				TR_ofDrawLine(Box.baseX + j * L, Box.baseY + (i + 1) * L, Box.baseX + (j + 1) * L, Box.baseY + (i + 1) * L);
		}
	}

	if (MArr[N].Orig.size() == 0) // 해당 MArr[N]의 미로를 처음으로 그릴 경우 미로 데이터로 Orig, WOrig, WType을 계산 (계산시 ScrX, ScrY를 사용함)
	{
		double X1 = Box.baseX + 0 * L, Y1 = Box.baseY + 0 * L, X2 = Box.baseX + MArr[N].Col * L, Y2 = Box.baseY + MArr[N].Row * L;
		int Row = MArr[N].Row, Col = MArr[N].Col;
		
		// 테두리 (외각 경계선)
		// 플레이어 충돌판정 (Orig)에 미로의 외각선 4개를 추가함 (두께가 2T인 Block형태로 저장)
		MArr[N].Orig.push_back(MakeBlock(X1, Y1 - T, X2, Y1 - T, X2, Y1 + T, X1, Y1 + T)); // 상
		MArr[N].Orig.push_back(MakeBlock(X2 - T, Y1, X2 + T, Y1, X2 + T, Y2, X2 - T, Y2)); // 우
		MArr[N].Orig.push_back(MakeBlock(X1, Y2 - T, X2, Y2 - T, X2, Y2 + T, X1, Y2 + T)); // 하
		MArr[N].Orig.push_back(MakeBlock(X1 - T, Y1, X1 + T, Y1, X1 + T, Y2, X1 - T, Y2)); // 좌

		// WOrig의 처리 (L >> T를 만족함)
		for (i = 0; i < Col; i++) // 가로 외각선 처리
		{
			double lx = 0, rx = 0, lx2 = 0, rx2 = 0; // 현재 칸에서 벽의 두께를 고려하여 추가할 선분의 길이 좌표 편차값 (0 : 벽없음, 다른값 : 벽있음)
			if (i > 0 && MArr[N].Maze[0][i - 1].r == 1) // 좌측에 벽 존재 (상)
				lx = T;
			if (i < Col - 1 && MArr[N].Maze[0][i].r == 1) // 우측에 벽 존재 (상)
				rx = -T;
			if (i > 0 && MArr[N].Maze[Row - 1][i - 1].r == 1) // 좌측에 벽 존재 (하)
				lx2 = T;
			if (i < Col - 1 && MArr[N].Maze[Row - 1][i].r == 1) // 우측에 벽 존재 (하)
				rx2 = -T;
			MArr[N].WOrig.push_back(MakeLine(X1 + L * i + lx, Y1, X1 + L * (i + 1) + rx, Y1)); // 계산된 길이로 가로선 추가
			MArr[N].Wtype.push_back(1); 
			MArr[N].WOrig.push_back(MakeLine(X1 + L * i + lx2, Y2, X1 + L * (i + 1) + rx2, Y2)); // 계산된 길이로 가로선 추가
			MArr[N].Wtype.push_back(1); 
		}

		for (i = 0; i < Row; i++) // 세로 외각선 처리
		{
			double uy = 0, dy = 0, uy2 = 0, dy2 = 0; // 현재 칸에서 벽의 두께를 고려하여 추가할 선분의 길이 좌표 편차값 (0 : 벽없음, 다른값 : 벽있음)
			if (i > 0 && MArr[N].Maze[i - 1][0].d == 1) // 위쪽에 벽 존재 (왼)
				uy = T;
			if (i < Row - 1 && MArr[N].Maze[i][0].d == 1) // 아래에 벽 존재 (왼)
				dy = -T;
			if (i > 0 && MArr[N].Maze[i - 1][Col - 1].d == 1) // 위쪽에 벽 존재 (우)
				uy2 = T;
			if (i < Row - 1 && MArr[N].Maze[i][Col - 1].d == 1) // 아래에 벽 존재 (우)
				dy2 = -T;
			MArr[N].WOrig.push_back(MakeLine(X1, Y1 + L * i + uy, X1, Y1 + L * (i + 1) + dy)); // 계산된 길이로 세로선 추가
			MArr[N].Wtype.push_back(0);
			MArr[N].WOrig.push_back(MakeLine(X2, Y1 + L * i + uy2, X2, Y1 + L * (i + 1) + dy2)); // 계산된 길이로 세로선 추가
			MArr[N].Wtype.push_back(0);
		}

		// 내부 벽 (미로의 각 방을 순회함), WOrig 계산시 Orig을 구성하는 4개의 선분들 중 서로 겹치지 않는 선분만 추출하여 추가함 (이후 같은 방향으로 서로 이어진 선분들은 모두 합쳐서 하나로 만듦)
		for (i = 0; i < Row; i++)
		{
			for (j = 0; j < Col; j++)
			{
				if (j < Col - 1 && MArr[N].Maze[i][j].r == 1) // 미로의 우측 벽이 있는 경우 (외각선 제외)
				{
					double X = Box.baseX + (j + 1) * L, Y1 = Box.baseY + (i)*L, Y2 = Box.baseY + (i + 1) * L;
					MArr[N].Orig.push_back(MakeBlock(X - T, Y1, X + T, Y1, X + T, Y2, X - T, Y2)); // 우측벽을 가로길이가 2T인 Block형태로 저장

					MArr[N].WOrig.push_back(MakeLine(X, Y1, X, Y2)); // 우측벽 중심에 내부 세로선 추가
					MArr[N].Wtype.push_back(5);

					int check1 = 1, check2 = 1, c1 = 0, c2 = 0; // 각각 위, 아래 두께 가로선 추가여부 (1:추가, 0:추가안함), 세로선의 길이를 두께만큼 잘라낼지의 여부 (위, 아래)
					if (i == 0) // 맨 윗줄
					{
						if (MArr[N].Maze[0][j].d == 1 || (j <= Col - 2 && MArr[N].Maze[0][j + 1].d == 1)) // 우측벽의 왼쪽아래((0,j).d), 또는 오른쪽 아래((0,j+1).d))가 있는 경우
						{
							// 세로선을 아래에서 두께(2T)만큼 줄이고, 아래쪽 가로 두께선을 추가하지 않는다.
							check2 = 0;
							c2 = 1;
						}
						if (c2 == 1) // 아래쪽 길이를 줄인 경우
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1, X - T, Y2 - T));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1, X + T, Y2 - T));
						}
						else // 줄이지 않은 경우
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1, X - T, Y2 + T));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1, X + T, Y2 + T));
						}
						MArr[N].Wtype.push_back(0); // 추가되는 두 선은 모두 세로선 (type = 0)
						MArr[N].Wtype.push_back(0);
					}
					else if (i == Row - 1) // 맨 아랫줄
					{
						if (MArr[N].Maze[Row - 2][j].d == 1 || (j <= Col - 2 && MArr[N].Maze[Row - 2][j + 1].d == 1)) // 우측벽의 왼쪽위((row-2,j).d), 또는 오른쪽 위((row-2,j+1).d))가 있는 경우
						{
							// 세로선을 위에서 두께(2T)만큼 줄이고, 위쪽 가로 두께선을 추가하지 않는다.
							check1 = 0;
							c1 = 1;
						}
						if (c1 == 1) // 위쪽 길이를 줄인 경우
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1 + T, X - T, Y2));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1 + T, X + T, Y2));
						}
						else  // 줄이지 않은 경우
						{
							MArr[N].WOrig.push_back(MakeLine(X - T, Y1 - T, X - T, Y2));
							MArr[N].WOrig.push_back(MakeLine(X + T, Y1 - T, X + T, Y2));
						}
						MArr[N].Wtype.push_back(0); // 추가되는 두 선은 모두 세로선 (type = 0)
						MArr[N].Wtype.push_back(0);
					}
					else 
					{
						if (MArr[N].Maze[i - 1][j].d == 1 || (j <= Col - 2 && MArr[N].Maze[i - 1][j + 1].d == 1)) // 우측벽의 왼쪽위((i-1,j).d), 또는 오른쪽 위((i-1,j+1).d))가 있는 경우
						{
							// 세로선을 위에서 두께(2T)만큼 줄이고, 위쪽 가로 두께선을 추가하지 않는다.
							check1 = 0;
							c1 = 1;
						}
						if (MArr[N].Maze[i][j].d == 1 || (j <= Col - 2 && MArr[N].Maze[i][j + 1].d == 1)) // 우측벽의 왼쪽아래((i,j).d), 또는 오른쪽 아래((i,j+1).d))가 있는 경우
						{
							// 세로선을 아래에서 두께(2T)만큼 줄이고, 아래쪽 가로 두께선을 추가하지 않는다.
							check2 = 0;
							c2 = 1;
						}

						// 줄인 길이에 맞게 선분을 추가함
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
						MArr[N].Wtype.push_back(0); // 추가되는 두 선은 모두 세로선 (type = 0)
						MArr[N].Wtype.push_back(0);
					}

					if (check1 == 1 && (i >= 1 && MArr[N].Maze[i - 1][j].r == 0)) // 오른쪽벽의 윗벽((i-1,j).r)이 없는 경우 위쪽 가로 두께선을 추가함 
					{
						MArr[N].WOrig.push_back(MakeLine(X - T, Y1 - T, X + T, Y1 - T));
						MArr[N].Wtype.push_back(2); // 추가되는 선은 가로 두께선 (type = 2)
					}

					if (check2 == 1 && (i <= Row - 2 && MArr[N].Maze[i + 1][j].r == 0)) // 오른쪽벽의 아래벽((i+1,j).r)이 없는 경우 아래쪽 가로 두께선을 추가함 
					{
						MArr[N].WOrig.push_back(MakeLine(X - T, Y2 + T, X + T, Y2 + T));
						MArr[N].Wtype.push_back(2); // 추가되는 선은 가로 두께선 (type = 2)
					}
				}
				if (i < Row - 1 && MArr[N].Maze[i][j].d == 1) // 아래쪽 벽이 있는 경우 (외각선 제외)
				{
					double X1 = Box.baseX + (j)*L, X2 = Box.baseX + (j + 1) * L, Y = Box.baseY + (i + 1) * L;
					MArr[N].Orig.push_back(MakeBlock(X1, Y - T, X2, Y - T, X2, Y + T, X1, Y + T)); // 세로 길이가 2T인 Block 형태로 저장 

					MArr[N].WOrig.push_back(MakeLine(X1, Y, X2, Y)); // 아래벽 내부에 내부 가로선 추가
					MArr[N].Wtype.push_back(4);

					int check1 = 1, check2 = 1; // 각각 왼쪽, 오른쪽 두께 세로선 추가여부 (1:추가, 0:추가안함)
					double lux = -T, ldx = -T, rux = T, rdx = T; // 두께만큼 방향에 맞춰 잘라낼때 사용할 길이
					if (j == 0) // 맨 왼쪽줄
					{
						if (MArr[N].Maze[i][0].r == 1) // 아래벽의 오른쪽 윗벽((i,0).r)이 있는 경우
						{
							// 위쪽 가로선을 오른쪽에서 두께(2T)만큼 줄이고, 오른쪽 세로 두께선을 추가하지 않는다.
							check2 = 0;
							rux = -T;
						}
						if ((i <= Row - 2 && MArr[N].Maze[i + 1][0].r == 1)) // 아래벽의 오른쪽 아랫벽((i+1,0).r)이 있는 경우
						{
							// 아래쪽 가로선을 오른쪽에서 두께(2T)만큼 줄이고, 오른쪽 세로 두께선을 추가하지 않는다.
							check2 = 0;
							rdx = -T;
						}
						MArr[N].WOrig.push_back(MakeLine(X1, Y - T, X2 + rux, Y - T)); // 줄인 길이에 맞게 선분을 추가함
						MArr[N].WOrig.push_back(MakeLine(X1, Y + T, X2 + rdx, Y + T));
						MArr[N].Wtype.push_back(1); // 추가되는 두 선은 모두 가로선 (type = 1)
						MArr[N].Wtype.push_back(1);
					}
					else if (j == Col - 1) // 맨 오른쪽줄
					{
						if (MArr[N].Maze[i][Col - 2].r == 1) // 아래벽의 왼쪽 윗벽((i,col-2).r)이 있는 경우
						{
							// 위쪽 가로선을 왼쪽에서 두께(2T)만큼 줄이고, 왼쪽 세로 두께선을 추가하지 않는다.
							check1 = 0;
							lux = T;
						}
						if ((i <= Row - 2 && MArr[N].Maze[i + 1][Col - 2].r == 1)) // 아래벽의 오른쪽 아랫벽((i+1,col-2).r)이 있는 경우
						{
							// 아래쪽 가로선을 왼쪽에서 두께(2T)만큼 줄이고, 왼쪽 세로 두께선을 추가하지 않는다.
							check1 = 0;
							ldx = T;
						}
						MArr[N].WOrig.push_back(MakeLine(X1 + lux, Y - T, X2, Y - T)); // 줄인 길이에 맞게 선분을 추가함
						MArr[N].WOrig.push_back(MakeLine(X1 + ldx, Y + T, X2, Y + T));
						MArr[N].Wtype.push_back(1); // 추가되는 두 선은 모두 가로선 (type = 1)
						MArr[N].Wtype.push_back(1);
					}
					else
					{
						if (MArr[N].Maze[i][j].r == 1)  // 아래벽의 오른쪽 윗벽((i,j).r)이 있는 경우
						{
							// 위쪽 가로선을 오른쪽에서 두께(2T)만큼 줄이고, 오른쪽 세로 두께선을 추가하지 않는다.
							check2 = 0;
							rux = -T;
						}
						if ((i <= Row - 2 && MArr[N].Maze[i + 1][j].r == 1))  // 아래벽의 오른쪽 아랫벽((i+1,j).r)이 있는 경우
						{
							// 아래쪽 가로선을 오른쪽에서 두께(2T)만큼 줄이고, 오른쪽 세로 두께선을 추가하지 않는다.
							check2 = 0;
							rdx = -T;
						}
						if (MArr[N].Maze[i][j - 1].r == 1) // 아래벽의 왼쪽 윗벽((i,j-1).r)이 있는 경우
						{
							// 위쪽 가로선을 왼쪽에서 두께(2T)만큼 줄이고, 왼쪽 세로 두께선을 추가하지 않는다.
							check1 = 0;
							lux = T;
						}
						if ((i <= Row - 2 && MArr[N].Maze[i + 1][j - 1].r == 1)) // 아래벽의 오른쪽 아랫벽((i+1,j-1).r)이 있는 경우
						{
							// 아래쪽 가로선을 왼쪽에서 두께(2T)만큼 줄이고, 왼쪽 세로 두께선을 추가하지 않는다.
							check1 = 0;
							ldx = T;
						}
						MArr[N].WOrig.push_back(MakeLine(X1 + lux, Y - T, X2 + rux, Y - T)); // 줄인 길이에 맞게 선분을 추가함
						MArr[N].WOrig.push_back(MakeLine(X1 + ldx, Y + T, X2 + rdx, Y + T));
						MArr[N].Wtype.push_back(1); // 추가되는 두 선은 모두 가로선 (type = 1)
						MArr[N].Wtype.push_back(1);
					}

					if (j >= 1 && MArr[N].Maze[i][j - 1].d == 0) // 아랫벽의 왼쪽벽((i,j-1).d)이 없는 경우 왼쪽 세로 두께선을 추가함 
					{
						MArr[N].WOrig.push_back(MakeLine(X1 - T, Y - T, X1 - T, Y + T));
						if (check1 == 0) // true면 해당 위치(왼쪽)에 세로벽이 존재함 -> 두께선이 아닌 세로선을 추가한다.
							MArr[N].Wtype.push_back(0); // 세로선 추가 (type = 0)
						else
							MArr[N].Wtype.push_back(2); // 두께선 추가 (type = 2)
					}
					if (i <= Col - 2 && MArr[N].Maze[i][j + 1].d == 0) // 아랫벽의 오른쪽벽((i,j+1).d)이 없는 경우 오른쪽 세로 두께선을 추가함 
					{
						MArr[N].WOrig.push_back(MakeLine(X2 + T, Y - T, X2 + T, Y + T));
						if (check2 == 0) // true면 해당 위치(오른쪽)에 세로벽이 존재함 -> 두께선이 아닌 세로선을 추가한다.
							MArr[N].Wtype.push_back(0); // 세로선 추가 (type = 0)
						else
							MArr[N].Wtype.push_back(2); // 두께선 추가 (type = 2)
					}
				}
			}
		}

		int count = 0, total = MArr[N].WOrig.size(); // WOrig의 총 길이
		vector<Line> &larr = MArr[N].WOrig; // 코드 간소화용 참조자 사용
		vector<int> &type = MArr[N].Wtype;

		// WOrig에 추가했던 선분들을 같은 방향으로 이어진것 끼리 서로 연결하여 추가하는 단계
		for (i = 0; i < total; i++) // WOrig 전체 순회
		{
			if (type[i] == 1) // 가로
			{
				while (1) {
					int check = 0;
					for (j = 0; j < total; j++) // 내부 순회
					{
						if (i != j && type[j] == 1) // 현재 선택된 선분(i)와 다른 선분이면서 타입이 같은(1) 선분인 경우
						{
							if (larr[i].y1 == larr[j].y1) // Y좌표가 서로 같을때
							{
								// j의 x좌표가 i의 선분 내부(경계점 포함)에 있거나 
								if (larr[i].x1 <= larr[j].x1 && larr[i].x2 >= larr[j].x1)
								{
									larr[i] = MakeLine(larr[i].x1, larr[i].y1, larr[j].x2, larr[i].y1);
									type[j] = 3;
									check = 1;
								}
								// i의 x좌표가 j의 선분 내부(경계점 포함)에 있으면 
								else if (larr[i].x1 >= larr[j].x1 && larr[i].x1 <= larr[j].x2)
								{
									// 하나의 선분을 만들고 선분 j에 만든 선분을 저장함 (i에는 type을 3으로 변경해 더이상 연산이 일어나지 않게 처리함)
									larr[i] = MakeLine(larr[j].x1, larr[i].y1, larr[i].x2, larr[i].y1);
									type[j] = 3;
									check = 1;
								}
							}
						}
					}
					if (check == 0) // 더이상 이을 선분이 없을때까지 반복 -> break
						break;
				}
			}
			else if (type[i] == 0) // 세로
			{
				while (1)
				{
					int check = 0;
					for (j = 0; j < total; j++)  // 내부 순회
					{
						if (i != j && type[j] == 0) // 현재 선택된 선분(i)와 다른 선분이면서 타입이 같은(0) 선분인 경우
						{
							if (larr[i].x1 == larr[j].x1) // X좌표가 서로 같을때
							{
								// j의 y좌표가 i의 선분 내부(경계점 포함)에 있거나  
								if (larr[i].y1 <= larr[j].y1 && larr[i].y2 >= larr[j].y1)
								{
									larr[j] = MakeLine(larr[i].x1, larr[i].y1, larr[i].x1, larr[j].y2);
									type[i] = 3;
								}
								// i의 y좌표가 j의 선분 내부(경계점 포함)에 있으면 
								else if (larr[i].y1 >= larr[j].y1 && larr[i].y1 <= larr[j].y2)
								{
									// 하나의 선분을 만들고 선분 j에 만든 선분을 저장함 (i에는 type을 3으로 변경해 더이상 연산이 일어나지 않게 처리함)
									larr[j] = MakeLine(larr[i].x1, larr[j].y1, larr[i].x1, larr[i].y2);
									type[i] = 3;
								}
							}
						}
					}
					if (check == 0) // 1회 순환시 type이 0이고, 선분 i와 이어진 모든 선분들은 연결이 완료됨 -> break
						break;
				}
			}

			else if (type[i] == 5) // 세로 (내부)
			{
				while (1)
				{
					int check = 0;
					for (j = 0; j < total; j++) // 내부 순회
					{
						if (i != j && type[j] == 5) // 현재 선택된 선분(i)와 다른 선분이면서 타입이 같은(5) 선분인 경우
						{
							if (larr[i].x1 == larr[j].x1) // X좌표가 서로 같을때
							{
								// j의 y좌표가 i의 선분 내부(경계점 포함)에 있거나  
								if (larr[i].y1 <= larr[j].y1 && larr[i].y2 >= larr[j].y1)
								{
									larr[j] = MakeLine(larr[i].x1, larr[i].y1, larr[i].x1, larr[j].y2);
									type[i] = 3;
								}
								// i의 y좌표가 j의 선분 내부(경계점 포함)에 있으면 
								else if (larr[i].y1 >= larr[j].y1 && larr[i].y1 <= larr[j].y2)
								{
									// 하나의 선분을 만들고 선분 j에 만든 선분을 저장함 (i에는 type을 3으로 변경해 더이상 연산이 일어나지 않게 처리함)
									larr[j] = MakeLine(larr[i].x1, larr[j].y1, larr[i].x1, larr[i].y2); 
									type[i] = 3;
								}
							}
						}
					}
					if (check == 0) // 1회 순환시 type이 5고, 선분 i와 이어진 모든 선분들은 연결이 완료됨 -> break
						break;
				}
			}

			if (type[i] == 4) // 가로 (내부)
			{
				while (1) {
					int check = 0;
					for (j = 0; j < total; j++) // 내부 순회
					{
						if (i != j && type[j] == 4) // 현재 선택된 선분(i)와 다른 선분이면서 타입이 같은(4) 선분인 경우
						{
							if (larr[i].y1 == larr[j].y1) // Y좌표가 서로 같을때 
							{
								// j의 x좌표가 i의 선분 내부(경계점 포함)에 있거나 
								if (larr[i].x1 <= larr[j].x1 && larr[i].x2 >= larr[j].x1)
								{
									larr[i] = MakeLine(larr[i].x1, larr[i].y1, larr[j].x2, larr[i].y1);
									type[j] = 3;
									check = 1;
								}
								// i의 x좌표가 j의 선분 내부(경계점 포함)에 있으면 
								else if (larr[i].x1 >= larr[j].x1 && larr[i].x1 <= larr[j].x2)
								{
									// 하나의 선분을 만들고 선분 j에 만든 선분을 저장함 (i에는 type을 3으로 변경해 더이상 연산이 일어나지 않게 처리함)
									larr[i] = MakeLine(larr[j].x1, larr[i].y1, larr[i].x2, larr[i].y1);
									type[j] = 3;
									check = 1;
								}
							}
						}
					}
					if (check == 0)  // 더이상 이을 선분이 없을때까지 반복 -> break
						break;
				}
			}

		}


		for (i = 0; i < larr.size(); i++) // Wall과 WTest의 size 계산
			if (type[i] != 3)
				count++;
		MArr[N].Trans.resize(MArr[N].Orig.size()); // Trans와 Orig의 크기는 같음 -> 벡터의 size 조절함
		// Wall과 WTest는 WOrig에서 Type이 3인 벽을 제외한 요소들을 저장 -> 벡터의 size를 조절함
		MArr[N].Wall.resize(count);
		MArr[N].WTest.resize(count);

	}

	if (Box.hintTimer > 0 && ofGetElapsedTimef() - Box.hintStartTime < 10.0) // 힌트를 사용했고, 힌트시간이 남은경우
	{
		Box.hintTimer = ofGetElapsedTimef() - Box.hintStartTime;
		DFS(N); // DFS 실행으로 경로 계산 (Path)
		dfsdraw(); // 경로를 그림
	}
}

//////////////////

int CheckMaze(int N) // 미로의 벽(플레이어-벽 충돌판정)과 충돌하는지 체크함
{
	int i, j, ret = 0;
	for (i = 0; i < MArr[N].Trans.size(); i++) // Trans를 순회하면서 미로의 모든 충돌판정 블럭과 만나는지를 체크함
	{
		if (CheckCollide(MArr[N].Trans[i]))
			ret = 1; // 하나라도 만나면 충돌한다로 판정함
	}
	return ret;
}

int CheckCircle(double X, double Y, double R)// Circle내부에 플레이어가 존재하는지 검사함 (마름모로 근사함)
{
	Block Temp; // 마름모로 근사
	Temp.P[0] = { X - R,Y - R };
	Temp.P[1] = { X + R,Y - R };
	Temp.P[2] = { X + R,Y + R };
	Temp.P[3] = { X - R,Y + R };
	return CheckCollide(Temp); // CheckCollide를 활용함
}

int CheckCollide(Block Target)  // Block내부에 플레이어가 존재하는지 검사함 (변환후 좌표를 기준으로함)
{
	int count = 0;
	for (int i = 0; i < 4; i++) { 
		int j = (i + 1) % 4; // i, j는 사각형의 각 변을 선택하게됨
		if ((Target.P[i].Y > Box.TLoc.Y) != (Target.P[j].Y > Box.TLoc.Y)) // 해당 변이 플레이어 좌표의 위쪽에 있거나 아래쪽에 있으면 넘어감
		{
			if (Target.P[j].Y == Target.P[i].Y) // 수평선인 경우의 처리
			{
				if (Target.P[i].Y == Box.TLoc.Y) // Y가 같으면 -> count 1 증가
					count++;
			}
			else
			{
				// 양쪽으로 수평선을 뻗어서 해당 변과 선이 만나는 지점을 찾아서 XLoc에 저장
				double XLoc = (Target.P[j].X - Target.P[i].X) * (Box.TLoc.Y - Target.P[i].Y) / (Target.P[j].Y - Target.P[i].Y) + Target.P[i].X;
				if (Box.TLoc.X < XLoc) // 오른쪽에서 만나면 -> Count 1 증가
					count++;
			}
		}
	}
	return count % 2; // 만나는 점이 홀수개 = 내부, 짝수개 = 외부
}

//////////////////

void DFS(int N) // DFS를 수행하는 함수
{
	int i, j;

	// 미로의 visited 초기화
	for (i = 0; i < MArr[N].Row; i++)
		for (j = 0; j < MArr[N].Col; j++)
			MArr[N].Maze[i][j].visited = 0;

	ClearStack(); // 스택과 Path를 초기화함
	Path.clear();

	int sx, sy; // 플레이어의 인덱스 위치 (DFS의 탐색 시작점이 됨)
	calc_Loc(&sx, &sy, N); // 플레이어의 현재 방 위치를 구함 (인덱스 위치)
	Push(sy, sx); // 시작점 위치
	MArr[N].Maze[sy][sx].visited = 1;
	while (Mtop != NULL) // iterative DFS 
	{
		i = Mtop->row;
		j = Mtop->col;

		if (i == MArr[N].Row - 1 && j == MArr[N].Col - 1) // 도착점 위치
			break;

		if (MArr[N].Maze[i][j].r == 0 && MArr[N].Maze[i][j + 1].visited == 0) // r 탐색
		{
			MArr[N].Maze[i][j + 1].visited = 1;
			Push(i, j + 1);
		}
		else if (MArr[N].Maze[i][j].d == 0 && MArr[N].Maze[i + 1][j].visited == 0) // d 탐색
		{
			MArr[N].Maze[i + 1][j].visited = 1;
			Push(i + 1, j);
		}
		else if (j > 0 && MArr[N].Maze[i][j - 1].r == 0 && MArr[N].Maze[i][j - 1].visited == 0) // l 탐색
		{
			MArr[N].Maze[i][j - 1].visited = 1;
			Push(i, j - 1);
		}
		else if (i > 0 && MArr[N].Maze[i - 1][j].d == 0 && MArr[N].Maze[i - 1][j].visited == 0) // u 탐색
		{
			MArr[N].Maze[i - 1][j].visited = 1;
			Push(i - 1, j);
		}
		else
			Pop(&i, &j); // 더이상 탐색할 경로 없음 -> Pop
	}

	int ni, nj; // 이전 위치
	Pop(&i, &j); // i, j에 도착점 위치 저장
	for (; Mtop != NULL;) // DFS의 결과로 스택에는 시작점부터 도착점까지의 경로가 저장됨
	{
		ni = Mtop->row; // 이전 위치 저장
		nj = Mtop->col;
		if (ni == i) // r 또는 l
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
		else if (nj == j) // d 또는 u
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

		Pop(&i, &j); // i, j 갱신
	}
}

void dfsdraw() // DFS로 구한 도착점까지의 경로를 출력함
{
	int i;
	ofSetColor(255, 128, 0); // 주황색으로 경로를 그림
	ofSetLineWidth(Box.zoom * 5);
	for (i = 0; i < Path.size(); i++) // Path를 순회하면서 내부 Line을 변환후 좌표로 그림
	{
		Line ltemp = Path.at(i);
		TR_ofDrawLine(ltemp.x1, ltemp.y1, ltemp.x2, ltemp.y2);
	}
}

//////////////////

void Push(int row, int col) // DFS용 스택 Push
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

void Pop(int* row, int* col) // DFS용 스택 Pop
{
	if (Mtop == NULL) // 남은 노드 없으면 종료
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

void ClearStack() // DFS용 스택을 초기화하는 함수
{
	for (; Mtop != NULL;) // 스택을 끝까지 순회하면서 할당해제함
	{
		Node* temp = Mtop;
		Mtop = Mtop->next;
		free(temp);
	}
	Mtop = NULL;
}
