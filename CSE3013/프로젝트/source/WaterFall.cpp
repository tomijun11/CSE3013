#include <iostream>
#include "ofApp.h"
#include "WaterFall.h"
#include "Maze.h"
#include "Matrix.h"
#include "Calc.h"

// ofApp.h 전역변수들 link
#define Stage_Num 5
extern MData MArr[Stage_Num];
extern Player Box;
extern double Matrix[3][3];
extern double Inverse[3][3];

void DrawWater(int N)  // 물줄기 및 용암 경로 계산후 그림
{
	for (int k = 0; k < MArr[N].Hole.size(); k++) // 물줄기 데이터 배열 순회
	{
		ofSetLineWidth(MArr[N].Hole[k].t); // 그릴 선의 굵기를 물줄기의 두께로 설정
		if (MArr[N].Hole[k].type == 0) // 물줄기 구멍 -> 파랑색 원
		{
			ofSetColor(0, 0, 255);
			TR_ofDrawCircle(MArr[N].Hole[k].Dot.X, MArr[N].Hole[k].Dot.Y, 10);
			ofSetColor(0, 255, 255); // 물줄기 색상 -> cyan
		}
		else // 용암 구멍 -> 어두운 빨간색 원
		{
			ofSetColor(128, 0, 0);
			TR_ofDrawCircle(MArr[N].Hole[k].Dot.X, MArr[N].Hole[k].Dot.Y, 10);
			ofSetColor(255, 0, 0); // 용암 색상 -> red
		}
		if (MArr[N].Hole[k].run == 1) // 물줄기가 실행중인 경우
		{
			Water_calcpath(N, MArr[N].Hole[k]); // 물줄기의 경로 계산 (Path와 wblock 계산)
			vector<Point>& Path = MArr[N].Hole[k].Path; // Path 순회 (물줄기 경로)
			for (int i = 0; i < MArr[N].Hole[k].size - 1; i++)
			{
				ofDrawLine(Path[i].X, Path[i].Y, Path[i + 1].X, Path[i + 1].Y); // 물줄기를 선으로 그림
			}
		}
	}
}

//////////////////

void Water_addpath(Water &Wobj, double X, double Y, int index) // Path에 물줄기의 경로(점)을 추가함
{
	if (index >= Wobj.Path.size()) // index(삽입위치)가 Path의 size에 도달한 경우
	{
		Wobj.Path.push_back({ X, Y }); // Path를 한 칸 늘려서 맨 뒤에 저장함
	}
	else
	{
		Wobj.Path[index] = { X,Y }; // Path의 끝에 도달하지 않은 경우 해당 위치에 값을 갱신함
	}
}

void Water_addpath(Water &Wobj, Point P, int index) // 바로 윗 함수의 overloading 버젼, X,Y대신 Point를 받음
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

void Water_addblock(Water& Wobj, Block B, int index) // wblock에 플레이어-물줄기 충돌판정 Block을 추가함
{
	if (index >= Wobj.wblock.size()) // index(삽입위치)가 wblock의 size에 도달한 경우
	{
		Wobj.wblock.push_back(B); // wblock를 한 칸 늘려서 맨 뒤에 저장함
	}
	else
	{
		Wobj.wblock[index] = B; // wblock의 끝에 도달하지 않은 경우 해당 위치에 값을 갱신함
	}
}

//////////////////

void Water_calcpath(int N, Water &Wobj) // 물줄기의 경로를 계산하는 함수
{
	int index = 0;
	Point TDot = Matrix_Trans(Matrix, Wobj.Dot); // 변환후 물줄기의 시작점 좌표를 구함
	Water_addpath(Wobj, TDot, index++); // 시작점을 경로에 추가
	double prevY = -1, prevX = -1, startflag = 0, curY = TDot.Y, curX = TDot.X; // 각각 이전 Y, X좌표, 경로탐색시작 flag, 현재 X, Y좌표
	int mini, count = 0, pi = -1; // CurrentY가 최소인 i값, 중복점 카운트, 이전에 선택한 i값
	for (int l = 0; l < 200; l++) // 최대 200번 반복 (무한루프 방지)
	{
		double minY = 100000; // 화면 Y좌표는 10만보다 작음
		int i, j, check = 0; // 각각 루프용 변수(i,j), 최소값 선택여부
		for (i = 0; i < MArr[N].Wall.size(); i++) // 미로의 물줄기 계산용 벽 배열을 순회함
		{
			Line Cur = MArr[N].Wall[i]; // 현재 선택된 벽
			if (curY > Cur.y1  && curY > Cur.y2) continue; // 벽의 시작점과 끝 점이 현재위치보다 위에 있거나
			if ((curX > Cur.x1) && (curX > Cur.x2)) continue; // 왼쪽에 있거나
			if ((curX < Cur.x1) && (curX < Cur.x2)) continue; // 오른쪽에 있거나
			// Slope 계산
			if (Cur.x1 == Cur.x2) continue; // 세로로 세워져있으면 스킵함

			double slope = (double)(Cur.y2 - Cur.y1) / (Cur.x2 - Cur.x1); // 벽의 기울기 계산
			double currentY = slope * (curX - Cur.x1) + Cur.y1; // 현재 위치에서 물줄기가 내려가서 벽에 닿을때의 Y좌표(currentY)를 계산함

			if (currentY < curY) // 닿은 좌표가 현재 Y보다 위에 있으면 스킵
				continue;

			if (MArr[N].WTest[i] <= 2 && startflag == 1) // 벽의 종류가 외부선 (가로,세로,두께)이고, 현재위치가 시작점이 아닌경우
			{
				if (curX == prevX && currentY == prevY) // 벽의 도달위치가 이전위치와 같은 경우
				{
					if (Cur.y1 >= Cur.y2) // ㄴ자 벽을 타고 흐르다가 꼭짓점에서 안떨어지고 ㄴ자의 가로선으로 바로 따라가는 경우를 방지함
					{
						if (prevY >= Cur.y2) // 이전위치보다 더 내려가지 않으면 스킵
							continue;
					}
					else
					{
						if (prevY >= Cur.y1) // 이전위치보다 더 내려가지 않으면 스킵 
							continue;
					}
				}
			}

			if (pi != i && currentY <= minY) // 이전에 선택한 벽과 다르고, 벽의 도달위치가 minY보다 위에 있는 경우
			{
				// 최소값 갱신
				minY = currentY; 
				mini = i;
				check = 1; // 최소값 계산 연산 체크
			}
		}
		if (check == 0 || (curY < prevY && startflag == 1)) // 최소값 계산 연산이 없거나 (아래에 외부선이 없음), 도달위치가 현재 위치보다 위로 올라가는 경우 경로추가를 중단함
			break;


		Line Cur = MArr[N].Wall[mini]; // Cur에 선택된 벽을 저장함
		// Path Added
		Water_addpath(Wobj, curX, minY, index++); // 벽 위의 도달점을 경로에 추가
		if (MArr[N].WTest[mini] >= 4) // 벽의 종류가 내부선 (내부 가로, 내부 세로선)인 경우 중단
			break;

		// 선분 끝점
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
		Water_addpath(Wobj, curX, curY, index++); // 벽의 양쪽점 중 더 아래에 있는 점을 경로에 추가

		if (curY == prevY && startflag == 1) // 현재위치가 시작점이 아니고, 이전점과 현재점의 Y좌표가 같은경우
		{
			if (count >= 1) // 2회이상 반복시 종료 (v자 계곡에 빠져서 더이상 진행X)
				break;
			else
				count++;
		}
		else 
			count = 0; // 탈출시 카운트 횟수를 초기화함

		prevY = curY; // 이전 좌표에 현재 좌표를 저장하고 다음 루프로 넘김
		prevX = curX;
		pi = mini;
		startflag = 1; 
	}
	Wobj.size = index; // size에 Path에 저장된 점의 갯수를 저장함

	// Path에 저장된 경로를 사용하여 wblock을 계산함
	double x1, x2, x3, x4, y1, y2, y3, y4, t = Wobj.t / 2;
	for (int i = 0; i < Wobj.size - 1; i++)
	{
		// Path에서 두 점씩 꺼내서 하나의 Block을 만듬
		Point P1 = Wobj.Path[i];
		Point P2 = Wobj.Path[i + 1];
		if (P1.Y == P2.Y) // 수평인 직사각형
		{
			x1 = P1.X; y1 = P1.Y - t;
			x2 = P1.X; y2 = P1.Y + t;
			x3 = P2.X; y3 = P2.Y - t;
			x4 = P2.X; y4 = P2.Y + t;
		}
		else // 기울기가 있는 직사각형 (좌표 연산으로 구함)
		{
			double slope = -(P2.X - P1.X) / (P2.Y - P1.Y), C = t / sqrt(1 + slope * slope);
			x1 = P1.X - C; y1 = slope * (x1 - P1.X) + P1.Y;
			x2 = P1.X + C; y2 = slope * (x2 - P1.X) + P1.Y;
			x3 = P2.X - C; y3 = slope * (x3 - P2.X) + P2.Y;
			x4 = P2.X + C; y4 = slope * (x4 - P2.X) + P2.Y;
		}
		Water_addblock(Wobj, MakeBlock(x1, y1, x2, y2, x4, y4, x3, y3), i); // 완성된 Block을 wblock에 추가함
	}
}
