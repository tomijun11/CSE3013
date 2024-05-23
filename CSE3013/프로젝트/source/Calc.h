#pragma once
#include <iostream>
#include "ofApp.h"
#include "WaterFall.h"
#include "Maze.h"

void calc_Loc(int* X, int* Y, int N); // ���� ��ġ���� �ε��� x, y�� ����� (��� �濡 �ִ����� ���)
double mapX(double X); // �ε��� X�� �ش��ϴ� X��ǥ�� �����
double mapY(double Y); // �ε��� Y�� �ش��ϴ� X��ǥ�� �����

Block MakeBlock(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4); // �� ������ block�� ������
Line MakeLine(double x1, double y1, double x2, double y2); // �� ������ ���� ������

void TR_ofDrawLine(double x1, double y1, double x2, double y2); // �߽��� (x,y), �������� r�� ���� ��ȯ�� ��ǥ�� �׸�
void TR_ofDrawCircle(double x, double y, double r); // �� �������� (x1,y1), (x2,y2)�� ������ ��ȯ�� ��ǥ�� �׸�
