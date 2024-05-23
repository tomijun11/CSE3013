#pragma once
#include <iostream>
#include "ofApp.h"
#include "Maze.h"

void DrawWater(int N); // ���ٱ� �� ��� ��� ����� �׸�

void Water_addpath(Water& Wobj, double X, double Y, int index);  // Path�� ���ٱ��� ���(��)�� �߰���
void Water_addpath(Water& Wobj, Point P, int index); // �ٷ� �� �Լ��� overloading ����, X,Y��� Point�� ����
void Water_addblock(Water& Wobj, Block B, int index);  // wblock�� �÷��̾�-���ٱ� �浹���� Block�� �߰���

void Water_calcpath(int N, Water& Wobj); // ���ٱ��� ��θ� ����ϴ� �Լ�

