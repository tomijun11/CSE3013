#pragma once
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "ofApp.h"

void Matrix_Clear(double(*A)[3]); // 변환행렬 초기화 함수
void Matrix_Mul(double(*A)[3], double(*B)[3], double(*C)[3]); // 두 행렬을 곱함 (C = A * B) 
void Matrix_Copy(double(*A)[3], double(*B)[3]); // 행렬을 복사함 (B = A)
double Matrix_GetDet2(double(*A)[2]); // 2x2 행렬의 determinant값을 구함
double Matrix_GetDet3(double(*A)[3]); // 3x3 행렬의 determinant값을 구함
int Matrix_Inverse(double(*A)[3], double(*B)[3]); // 역행렬을 구함 (B = A^-1)
Point Matrix_Trans(double(*A)[3], double X, double Y); // X,Y 좌표를 행렬A로 변환한 값을 구함
Point Matrix_Trans(double(*A)[3], Point P); // 바로 위 함수의 overloading 버젼, X,Y대신 Point로 받음
Block Matrix_TransBlock(double(*A)[3], Block B); // Block을 행렬A로 변환한 값을 구함
Line Matrix_TransLine(double(*A)[3], Line L); // Line을 행렬A로 변환한 값을 구함

void Stack_Move(double X, double Y); // 변환행렬에 평행이동 행렬을 곱하는 함수
void Stack_Scale(double X, double Y); // 변환행렬에 크기조절 행렬을 곱하는 함수
void Stack_Reflect(double X, double Y); // 변환행렬에 반사변환 행렬을 곱하는 함수
void Stack_Rotate(double Angle); // 변환행렬에 회전변환 행렬을 곱하는 함수
void Stack_RotateAt(double Angle, double X, double Y); // 변환행렬에 회전의 중심을 X,Y로 하는 회전변환 행렬을 곱하는 함수
void Stack_Shear(double X, double Y); // 변환행렬에 전단변형 행렬을 곱하는함수
int Stack_Project(int N, double LUX, double LUY, double LDX, double LDY, double RUX, double RUY, double RDX, double RDY); // 변환행렬에 투영행렬을 곱하는 함수