#pragma once
#include <iostream>
#include "ofApp.h"
#include "WaterFall.h"

void DrawMaze(int N); // Draw Maze (힌트 사용시 DFS 사용으로 힌트도 출력함, 첫 호출시 현재 화면 크기를 기준으로 미로의 Orig, WOrig, WType을 계산함)

int CheckMaze(int N); // 미로의 벽(플레이어-벽 충돌판정)과 충돌하는지 체크함
int CheckCollide(Block Target); // Block내부에 플레이어가 존재하는지 검사함
int CheckCircle(double X, double Y, double R); // Circle내부에 플레이어가 존재하는지 검사함 (마름모로 근사함)

void DFS(int N); // DFS를 수행하는 함수
void dfsdraw(); // DFS로 구한 도착점까지의 경로를 출력함

void Push(int row, int col); // DFS용 스택 Push
void Pop(int* row, int* col); // DFS용 스택 Pop
void ClearStack(); // DFS용 스택을 초기화하는 함수