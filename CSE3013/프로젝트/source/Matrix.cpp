#pragma once
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "Matrix.h"
#include "ofApp.h"
#define PI 3.14159265

// ofApp.h ���������� link
#define Stage_Num 5
extern double Matrix[3][3];
extern MData MArr[Stage_Num];
extern Player Box;

// ������� ���
void Matrix_Clear(double(*A)[3])  // ��ȯ��� �ʱ�ȭ �Լ� (A = I)
{
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            if (i == j)
                A[i][j] = 1;
            else
                A[i][j] = 0;
}

void Matrix_Mul(double(*A)[3], double(*B)[3], double(*C)[3]) // �� ����� ���� (C = A * B) 
{
	double temp[3][3];
	double sum;
	for (int i = 0; i < 3; i++) // row
	{
		for (int j = 0; j < 3; j++) // col
		{
			sum = 0;
			for (int k = 0; k < 3; k++)
				sum += A[i][k] * B[k][j];
			temp[i][j] = sum;
		}
	}
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			C[i][j] = temp[i][j];
}


void Matrix_Copy(double(*A)[3], double(*B)[3]) // ����� ������ (B = A)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			B[i][j] = A[i][j];
}

double Matrix_GetDet2(double(*A)[2]) // 2x2 ����� determinant���� ����
{
    return A[0][0] * A[1][1] - A[0][1] * A[1][0]; // ad-bc
}

double Matrix_GetDet3(double(*A)[3])  // 3x3 ����� determinant���� ����
{
    return A[0][0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1])
        - A[0][1] * (A[1][0] * A[2][2] - A[1][2] * A[2][0])
        + A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
}

int Matrix_Inverse(double(*A)[3], double(*B)[3])  // ������� ���� (B = A^-1)
{
    double detA = Matrix_GetDet3(A); 
    if (detA == 0) return -1; // ������� ���°�� -1�� ��ȯ

    double CT[3][3];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            double M[2][2];
            int row = 0, col = 0;
            for (int u = 0; u < 3; u++)
            {
                if (u == i)
                    continue;
                else
                {
                    for (int v = 0; v < 3; v++)
                        if (v == j)
                            continue;
                        else
                        {
                            if (row > 1) goto Coef;
                            M[row][col] = A[u][v];
                            col++;
                            if (col > 1) col = 0;
                        }
                    row++;
                }
            }
        Coef:
            double temp = Matrix_GetDet2(M);
            if ((i + j) % 2 == 1)
                temp = -temp;
            CT[j][i] = temp / detA;
        }
    }

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            B[i][j] = CT[i][j];

    return 0;
}

Point Matrix_Trans(double(*A)[3], double X, double Y) // X,Y ��ǥ�� ���A�� ��ȯ�� ���� ����
{
    double temp[3];
    double x[3] = { X, Y, 1 };
    double sum;
    for (int i = 0; i < 3; i++)
    {
        sum = 0;
        for (int j = 0; j < 3; j++)
            sum += A[i][j] * x[j];
        temp[i] = sum;
    }
    Point ret;
    ret.X = temp[0] / temp[2];
    ret.Y = temp[1] / temp[2];
    return ret;
}

Point Matrix_Trans(double(*A)[3], Point P) // �ٷ� �� �Լ��� overloading ����, X,Y��� Point�� ����
{
    double temp[3];
    double x[3] = { P.X, P.Y, 1 };
    double sum;
    for (int i = 0; i < 3; i++)
    {
        sum = 0;
        for (int j = 0; j < 3; j++)
            sum += A[i][j] * x[j];
        temp[i] = sum;
    }
    Point ret;
    ret.X = temp[0] / temp[2];
    ret.Y = temp[1] / temp[2];
    return ret;
}

Block Matrix_TransBlock(double(*A)[3], Block B) // Block�� ���A�� ��ȯ�� ���� ����
{
    Block ret;
    for (int i = 0; i < 4; i++)
        ret.P[i] = Matrix_Trans(A, B.P[i]);
    return ret;
}

Line Matrix_TransLine(double(*A)[3], Line L) // Line�� ���A�� ��ȯ�� ���� ����

{
    Point P1 = Matrix_Trans(A, L.x1, L.y1);
    Point P2 = Matrix_Trans(A, L.x2, L.y2);
    Line ret = { P1.X,P1.Y,P2.X,P2.Y };
    return ret;
}



void Stack_Move(double X, double Y) // ��ȯ��Ŀ� �����̵� ����� ���ϴ� �Լ�
{
	double temp[3][3], newA[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
	Matrix_Copy(Matrix, temp);

	newA[0][2] = X;
	newA[1][2] = Y;
	Matrix_Mul(newA, Matrix, Matrix);
}

void Stack_Scale(double X, double Y) // ��ȯ��Ŀ� ũ������ ����� ���ϴ� �Լ�
{
	double temp[3][3], newA[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
	Matrix_Copy(Matrix, temp);

	newA[0][0] = X;
	newA[1][1] = Y;
	Matrix_Mul(newA, Matrix, Matrix);
}

void Stack_Reflect(double X, double Y) // ��ȯ��Ŀ� �ݻ纯ȯ ����� ���ϴ� �Լ�
{
    double temp[3][3], newA[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
    Matrix_Copy(Matrix, temp);

    (abs(X) == 0) ? newA[0][0] = 1 : newA[0][0] = -1;
    (abs(Y) == 0) ? newA[1][1] = 1 : newA[1][1] = -1;
    Matrix_Mul(newA, Matrix, Matrix);
}

void Stack_Rotate(double Angle) // ��ȯ��Ŀ� ȸ����ȯ ����� ���ϴ� �Լ�
{
    double temp[3][3], newA[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
    Matrix_Copy(Matrix, temp);

    Angle *= PI / 180;
    newA[0][0] = cos(Angle); newA[0][1] = -sin(Angle);
    newA[1][0] = sin(Angle); newA[1][1] = cos(Angle);
    Matrix_Mul(newA, Matrix, Matrix);
}

void Stack_RotateAt(double Angle, double X, double Y) // ��ȯ��Ŀ� ȸ���� �߽��� X,Y�� �ϴ� ȸ����ȯ ����� ���ϴ� �Լ�
{
    double temp[3][3], newA[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
    double newT[3][3] = { {1,0,-X},{0,1,-Y},{0,0,1} };
    double InvT[3][3] = { {1,0,X},{0,1,Y},{0,0,1} };
    Matrix_Copy(Matrix, temp);

    Angle *= PI / 180;
    newA[0][0] = cos(Angle); newA[0][1] = -sin(Angle);
    newA[1][0] = sin(Angle); newA[1][1] = cos(Angle);
    // M = Move, A = Rotate��, MAM^-1�� ����
    Matrix_Mul(newT, Matrix, Matrix);
    Matrix_Mul(newA, Matrix, Matrix);
    Matrix_Mul(InvT, Matrix, Matrix);
}

void Stack_Shear(double X, double Y) // ��ȯ��Ŀ� ���ܺ��� ����� ���ϴ��Լ�
{
    double temp[3][3], newA[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
    Matrix_Copy(Matrix, temp);

    newA[0][1] = X;
    newA[1][0] = Y;
    Matrix_Mul(newA, Matrix, Matrix);
}

int Stack_Project(int N, double LUX, double LUY, double LDX, double LDY, double RUX, double RUY, double RDX, double RDY) // ��ȯ��Ŀ� ��������� ���ϴ� �Լ�
{
    double temp[3][3], newA[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
    Matrix_Copy(Matrix, temp);

    double X = Box.baseX + Box.L * MArr[N].Col, Y = Box.baseY + Box.L * MArr[N].Row; // ���� ������ ���� ������ �̷��� ũ��� ������ (0~X, 0~Y)

    newA[0][2] = LUX; // c
    newA[1][2] = LUY; // f
    double GHA[2][2] = {
        {X * (RUX - RDX), Y * (LDX - RDX)},
        {X * (RUY - RDY), Y * (LDY - RDY)}
    };
    double GHb[2] = {
        RDX - RUX - LDX + LUX,
        RDY - RUY - LDY + LUY
    };
    double GHI[2][2], detGH = Matrix_GetDet2(GHA);
    if (GHI == 0) return 1;
    GHI[0][0] = GHA[1][1] / detGH;
    GHI[0][1] = -GHA[0][1] / detGH;
    GHI[1][0] = -GHA[1][0] / detGH;
    GHI[1][1] = GHA[0][0] / detGH;

    newA[2][0] = GHI[0][0] * GHb[0] + GHI[0][1] * GHb[1]; // g
    newA[2][1] = GHI[1][0] * GHb[0] + GHI[1][1] * GHb[1]; // h

    newA[0][0] = RUX * newA[2][0] + (RUX - LUX) / X; // a
    newA[1][0] = RUY * newA[2][0] + (RUY - LUY) / X; // d
    newA[0][1] = LDX * newA[2][1] + (LDX - LUX) / Y; // b
    newA[1][1] = LDY * newA[2][1] + (LDY - LUY) / Y; // e

    Matrix_Mul(newA, Matrix, Matrix);
    return 0;
}
