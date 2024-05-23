#pragma once
#include <iostream>
#include "ofApp.h"
#include "WaterFall.h"
#include "Maze.h"

void calc_Loc(int* X, int* Y, int N); // 현재 위치에서 인덱스 x, y를 계산함 (어느 방에 있는지를 계산)
double mapX(double X); // 인덱스 X에 해당하는 X좌표를 계산함
double mapY(double Y); // 인덱스 Y에 해당하는 X좌표를 계산함

Block MakeBlock(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4); // 네 점으로 block을 생성함
Line MakeLine(double x1, double y1, double x2, double y2); // 두 점으로 선을 생성함

void TR_ofDrawLine(double x1, double y1, double x2, double y2); // 중심이 (x,y), 반지름이 r인 원을 변환후 좌표로 그림
void TR_ofDrawCircle(double x, double y, double r); // 두 꼭짓점이 (x1,y1), (x2,y2)인 선분을 변환후 좌표로 그림
