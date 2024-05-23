#pragma once
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "ofApp.h"

void Matrix_Clear(double(*A)[3]); // ��ȯ��� �ʱ�ȭ �Լ�
void Matrix_Mul(double(*A)[3], double(*B)[3], double(*C)[3]); // �� ����� ���� (C = A * B) 
void Matrix_Copy(double(*A)[3], double(*B)[3]); // ����� ������ (B = A)
double Matrix_GetDet2(double(*A)[2]); // 2x2 ����� determinant���� ����
double Matrix_GetDet3(double(*A)[3]); // 3x3 ����� determinant���� ����
int Matrix_Inverse(double(*A)[3], double(*B)[3]); // ������� ���� (B = A^-1)
Point Matrix_Trans(double(*A)[3], double X, double Y); // X,Y ��ǥ�� ���A�� ��ȯ�� ���� ����
Point Matrix_Trans(double(*A)[3], Point P); // �ٷ� �� �Լ��� overloading ����, X,Y��� Point�� ����
Block Matrix_TransBlock(double(*A)[3], Block B); // Block�� ���A�� ��ȯ�� ���� ����
Line Matrix_TransLine(double(*A)[3], Line L); // Line�� ���A�� ��ȯ�� ���� ����

void Stack_Move(double X, double Y); // ��ȯ��Ŀ� �����̵� ����� ���ϴ� �Լ�
void Stack_Scale(double X, double Y); // ��ȯ��Ŀ� ũ������ ����� ���ϴ� �Լ�
void Stack_Reflect(double X, double Y); // ��ȯ��Ŀ� �ݻ纯ȯ ����� ���ϴ� �Լ�
void Stack_Rotate(double Angle); // ��ȯ��Ŀ� ȸ����ȯ ����� ���ϴ� �Լ�
void Stack_RotateAt(double Angle, double X, double Y); // ��ȯ��Ŀ� ȸ���� �߽��� X,Y�� �ϴ� ȸ����ȯ ����� ���ϴ� �Լ�
void Stack_Shear(double X, double Y); // ��ȯ��Ŀ� ���ܺ��� ����� ���ϴ��Լ�
int Stack_Project(int N, double LUX, double LUY, double LDX, double LDY, double RUX, double RUY, double RDX, double RDY); // ��ȯ��Ŀ� ��������� ���ϴ� �Լ�