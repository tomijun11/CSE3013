#pragma once
#include <iostream>
#include "ofApp.h"
#include "Maze.h"

void DrawWater(int N); // 물줄기 및 용암 경로 계산후 그림

void Water_addpath(Water& Wobj, double X, double Y, int index);  // Path에 물줄기의 경로(점)을 추가함
void Water_addpath(Water& Wobj, Point P, int index); // 바로 윗 함수의 overloading 버젼, X,Y대신 Point를 받음
void Water_addblock(Water& Wobj, Block B, int index);  // wblock에 플레이어-물줄기 충돌판정 Block을 추가함

void Water_calcpath(int N, Water& Wobj); // 물줄기의 경로를 계산하는 함수

