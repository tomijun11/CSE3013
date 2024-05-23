#pragma once
#include <iostream>
#include "ofApp.h"
#include "WaterFall.h"

void DrawMaze(int N); // Draw Maze (��Ʈ ���� DFS ������� ��Ʈ�� �����, ù ȣ��� ���� ȭ�� ũ�⸦ �������� �̷��� Orig, WOrig, WType�� �����)

int CheckMaze(int N); // �̷��� ��(�÷��̾�-�� �浹����)�� �浹�ϴ��� üũ��
int CheckCollide(Block Target); // Block���ο� �÷��̾ �����ϴ��� �˻���
int CheckCircle(double X, double Y, double R); // Circle���ο� �÷��̾ �����ϴ��� �˻��� (������� �ٻ���)

void DFS(int N); // DFS�� �����ϴ� �Լ�
void dfsdraw(); // DFS�� ���� ������������ ��θ� �����

void Push(int row, int col); // DFS�� ���� Push
void Pop(int* row, int* col); // DFS�� ���� Pop
void ClearStack(); // DFS�� ������ �ʱ�ȭ�ϴ� �Լ�