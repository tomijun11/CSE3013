#include <iostream>
#include "ofApp.h"
#include "WaterFall.h"
#include "Maze.h"
#include "Matrix.h"

// ofApp.h 전역변수들 link
#define Stage_Num 5
extern MData MArr[Stage_Num];
extern Node* Mtop;
extern Player Box;
extern double Matrix[3][3];
extern double Inverse[3][3];

void calc_Loc(int* X, int* Y, int N) // 현재 위치에서 인덱스 x, y를 계산함 (어느 방에 있는지를 계산)
{
	*X = (Box.Loc.X - Box.baseX) / Box.L;
	if (*X < 0) *X = 0;
	if (*X > MArr[N].Col) *X = MArr[N].Col;
	*Y = (Box.Loc.Y - Box.baseY) / Box.L;
	if (*Y < 0) *Y = 0;
	if (*Y > MArr[N].Row) *Y = MArr[N].Row;
}

double mapX(double X) // 인덱스 X에 해당하는 X좌표를 계산함
{
	return Box.baseX + Box.L * X;
}

double mapY(double Y) // 인덱스 Y에 해당하는 X좌표를 계산함
{
	return Box.baseY + Box.L * Y;
}
//////////////////

Line MakeLine(double x1, double y1, double x2, double y2) // 두 점으로 선을 생성함
{
	Line ret;
	ret.x1 = x1;
	ret.x2 = x2;
	ret.y1 = y1;
	ret.y2 = y2;
	return ret;
}

Block MakeBlock(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) // 네 점으로 block을 생성함
{
	Block ret;
	ret.P[0] = { x1,y1 };
	ret.P[1] = { x2,y2 };
	ret.P[2] = { x3,y3 };
	ret.P[3] = { x4,y4 };
	return ret;
}

//////////////////

void TR_ofDrawCircle(double x, double y, double r) // 중심이 (x,y), 반지름이 r인 원을 변환후 좌표로 그림
{
	Point P = Matrix_Trans(Matrix, x, y);
	ofDrawCircle(P.X, P.Y, r);
}

void TR_ofDrawLine(double x1, double y1, double x2, double y2) // 두 꼭짓점이 (x1,y1), (x2,y2)인 선분을 변환후 좌표로 그림
{
	Point P1 = Matrix_Trans(Matrix, x1, y1);
	Point P2 = Matrix_Trans(Matrix, x2, y2);
	ofDrawLine(P1.X, P1.Y, P2.X, P2.Y);
}

