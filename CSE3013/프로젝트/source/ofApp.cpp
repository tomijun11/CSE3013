#include "ofApp.h"
#include "Matrix.h"
#include "WaterFall.h"
#include "Maze.h"
#include "Calc.h"
#include <iostream>
#include <cmath>
using namespace std;

Node* Mtop = NULL; // DFS�� ������ Top
#define Length 30.0 // �̷� �� ĭ�� �⺻ ũ��
FILE* fptr; // ���̺� ���� ������
#define Stage_Num 5 // �� �������� ��
MData MArr[Stage_Num]; // �̷� �����͸� �����ϴ� �迭
vector<Line> Path; // ��Ʈ ��θ� �����ϴ� �迭
Player Box; // �÷��̾� ������ ����
int Stage_Clear, Star_Clear[Stage_Num], Screen=1, prevScreen; // ���� Ŭ���� �� �������� ��, �� ���������� �ְ� ���� ����, ���� ȭ�� ��ȣ, ���� ȭ�� ��ȣ
double Time_Clear[Stage_Num], Limit_Clear[Stage_Num] = {60.0,90.0,60.0,90.0,45.0}; // ���� Ŭ������ �ִܽð�, ���������� �ð�����
ofTrueTypeFont myFont24, myFont16, myFont12, myFont48, myFont60, myFont36, myFont20; // ��Ʈ�� ����
Button BArr[Stage_Num]; // ��ư�� ������ �����ϴ� �迭
double Matrix[3][3]; // ��ȯ�� ���
double Inverse[3][3] = { {1,0,0},{0,1,0},{0,0,1} }; // ����ȯ�� ���
int WaterNum[Stage_Num] = { 2,4,10,12,8 }; // ���������� ���ٱ� ����
double ScrX[Stage_Num], ScrY[Stage_Num]; // DrawMaze ù ȣ��� ȭ�� ũ�� (���� ����, ����ũ��)

//--------------------------------------------------------------
void ofApp::setup() {

	// �̹��� �ε�
	Title.load("title.png");
	StageTitle.load("stage.png");
	tipTitle.load("tip.png");
	tip.load("tip2.png");

	ofSetWindowTitle("Maze Runner"); // ������ â �̸� ����
	ofSetFrameRate(30); // �ʴ� ������ �� ����
	ofBackground(255, 255, 255);
	
	// ������ â ��ġ ����
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);
	
	// �������� ��ư ��ġ �� �̸� ����
	char buf[Stage_Num]; 
	for (int i = 0; i < Stage_Num; i++)
	{
		strcpy_s(BArr[i].name, "Stage ");
		strcat_s(BArr[i].name, itoa(i + 1, buf, 10));
		BArr[i].X = 120 + 160 * (i % 5);
		BArr[i].Y = 320;
	}

	// ��Ʈ �ε�
	myFont12.loadFont("verdana.ttf", 12, true, true);
	myFont16.loadFont("verdana.ttf", 16, true, true);
	myFont20.loadFont("verdana.ttf", 20, true, true);
	myFont24.loadFont("verdana.ttf", 24, true, true);
	myFont36.loadFont("verdana.ttf", 36, true, true);
	myFont48.loadFont("verdana.ttf", 48, true, true);
	myFont60.loadFont("verdana.ttf", 60, true, true);

	// Load save.txt
	char savename[] = {"save.txt"};
	fopen_s(&fptr, ofToDataPath(savename).c_str(), "r");
	// save.txt ���°�� ������ �����ϰ� ��ε�
	if (fptr == NULL)
	{
		fopen_s(&fptr, ofToDataPath(savename).c_str(), "w");
		fprintf(fptr, "0\n"); // total clear Stage
		for (int i = 0; i<Stage_Num; i++)
			fprintf(fptr, "0 0\n"); // Star, Time
		fclose(fptr);

		fopen_s(&fptr, ofToDataPath(savename).c_str(), "r");
	}
	fscanf(fptr, "%d", &Stage_Clear);
	for (int i = 0; i < Stage_Num; i++)
		fscanf(fptr, "%d %lf", &(Star_Clear[i]), &(Time_Clear[i]));
	fclose(fptr);

	// Load maze files
	FILE* fmaze;
	char fname[50] = { "maze0.maz" }; // fname[4]�� �ٲ�
	for (int i = 0; i < Stage_Num; i++) // �� ���������� �̷� ������ �ε��ؼ� MArr[i]�� ������
	{
		fname[4] = '0' + i;
		fopen_s(&fmaze, ofToDataPath(fname).c_str(), "r"); // .maz���� �ε�
		
		char templine[100] = { 0 };
		fgets(templine, 100, fmaze); // 1���� �ҷ���
		templine[strlen(templine) - 1] = '\0'; 

		MArr[i].Col = (strlen(templine) - 1) / 2; // .maz�� �� ���� ���̴� 2*width+1 (\0����) -> width�� ������
		MArr[i].Row = 0;
		while (1) // feof ���޽ñ��� ���پ� �����鼭 Row�� ���� -> .maz�� ���� �� ������ �����
		{
			fgets(templine, 100, fmaze);
			templine[strlen(templine) - 1] = '\0';
		
			if (feof(fmaze))
				break;
			MArr[i].Row++;
		}
		if (strlen(templine) > 1)
			MArr[i].Row++;
		MArr[i].Row /= 2; // .maz�� ���� ������ 2*height+1 (eof����) -> height�� ������

		// Maze ������ �Ҵ�
		int j, k;
		MArr[i].Maze = (Room**)malloc(sizeof(Room*) * MArr[i].Row); 
		for (j = 0; j < MArr[i].Row; j++)
			MArr[i].Maze[j] = (Room*)malloc(sizeof(Room) * MArr[i].Col);

		// Maze ������ �ʱ�ȭ
		for (k = 0; k < MArr[i].Row; k++) 
			for (j = 0; j < MArr[i].Col; j++)
			{
				MArr[i].Maze[k][j].visited = 0;
				MArr[i].Maze[k][j].r = 1;
				MArr[i].Maze[k][j].d = 1;
			}

		fseek(fmaze, 0, 0); // ���� �����͸� �������� �ǵ���
		fgets(templine, 100, fmaze); // �� ���� �ѱ��
		for (k = 0; k < MArr[i].Row; k++) // �ѹ��� 2�پ� ����, ������ �� ���� ������ ��(2*j+2), �Ʒ����� �� ���� �Ʒ��� ��(2*j+1)�� ������ ������� -> MArr[i].Maze�� �� ����
		{
			char rline[100] = { 0 }; // ����
			char dline[100] = { 0 }; // �Ʒ���
			fgets(rline, 100, fmaze); 
			fgets(dline, 100, fmaze);

			for (j = 0; j < MArr[i].Col; j++) // �� ���� �� ���� (Col�� �ݺ�)
			{
				// fill rwall
				if (j < MArr[i].Col - 1) // �� �����ʹ� : ������ ���� �׻� ���� ����
				{
					if (rline[2 * j + 2] == ' ')
						MArr[i].Maze[k][j].r = 0;
				}

				// fill dwall
				if (k < MArr[i].Row - 1) // �̷��� �� �Ʒ��� : �Ʒ��� ���� �׻� ���� ����
				{
					if (dline[2 * j + 1] == ' ')
						MArr[i].Maze[k][j].d = 0;
				}
			}
		}
		fclose(fmaze);

		// �̷��� ���ٱ� ���� �ʱ�ȭ
		MArr[i].Hole.resize(WaterNum[i]);
		for (int k = 0; k < WaterNum[i]; k++)
		{
			MArr[i].Hole[k].size = 0;
		}
	}
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	/* Screen : ȭ�� ��� ��� ����
		1 : ����ȭ��
		2 : ���Ӽ���
		3 : �޴�ȭ��
		4 : ���ӿ���
		5 : �¸�ȭ��
		10~14 : �ش� �������� ȭ��
	*/
	int XCntr = ofGetWidth() / 2, YCntr = ofGetHeight(), i, j, rtemp; // XCntr : ȭ���� ������ǥ (XCntr : X��ǥ, YCntr : Y��ǥ), �������� �ӽú���
	double score; // ���� ���� �ӽú���
	switch (Screen) // �� ȭ�� ��½� ������ �ڵ�
	{
	case 1:
		// ����ȭ�� : ��� ���
		ofSetBackgroundColor(255, 255, 255);
		// ���� ���� ���
		ofSetColor(255);
		Title.draw(XCntr - 400, 120);
		// ��ư�� �׸��� 
		ofSetColor(0);
		ofDrawRectRounded(XCntr - 130+5, 440+5, 260, 50, 25);
		ofDrawRectRounded(XCntr - 130+5, 520+5, 260, 50, 25);
		// ��ư ��ü
		ofSetColor(192);
		ofDrawRectRounded(XCntr - 130 , 440, 260, 50, 25);
		ofDrawRectRounded(XCntr - 130, 520, 260, 50, 25);
		// ��ư �ؽ�Ʈ
		ofSetColor(0, 0, 0);
		myFont16.drawString("press S to Start", XCntr - 80, 470);
		myFont16.drawString("How To Play (H)", XCntr - 80, 550);
		break;
	case 2:
		// ���Ӽ��� ȭ�� : ��� ���
		ofSetBackgroundColor(255, 255, 255);
		// ���� ���� ���� ���
		ofSetColor(255);
		tipTitle.draw(XCntr - 200, 10); // ����
		tip.draw(XCntr - 400, 100); // ����
		// ��ư�� �׸���
		ofSetColor(0);
		ofDrawRectRounded(XCntr - 130+5, 660+5, 260, 50, 25);
		// ��ư ��ü
		ofSetColor(192);
		ofDrawRectRounded(XCntr - 130, 660, 260, 50, 25);
		// ��ư �ؽ�Ʈ
		ofSetColor(0, 0, 0);
		myFont16.drawString("press S to Start", XCntr - 80, 690);
		break;
	case 3:
		// �޴�ȭ�� : ȸ�� ���
		ofSetBackgroundColor(224, 224, 224);
		// �޴� ���� ��� (Select Stage)
		ofSetColor(255);
		StageTitle.draw(XCntr - 290, 100);
		// ��ư ���
		char TBuf[100]; // �ð��� ��¿� ����
		for (int i = 0; i < Stage_Num; i++) // �� ���������� ��ư ���
		{
			// ��ư�� �׸���
			ofSetColor(64);
			ofDrawRectRounded(XCntr-512+BArr[i].X+5, BArr[i].Y+5, 140, 180, 20);
			// ��ư ��ü
			ofSetColor(255, 255, 255); // �رݽ� ���
			if (Stage_Clear < i) ofSetColor(0, 255, 255); // ���رݽ� cyan��
			ofDrawRectRounded(XCntr - 512 + BArr[i].X, BArr[i].Y, 140, 180, 20);
			// ��ư�� �̸� (�������� �̸�)
			ofSetColor(0,0,255);
			myFont20.drawString(BArr[i].name, XCntr - 512 + BArr[i].X+15, BArr[i].Y +35);
			// �ִ� ���� ���� ���
			ofSetColor(255, 255, 0); // ���� ���� : �����, ��ȹ�� ������ ��� ȸ������ ĥ����
			if (Star_Clear[i] < 1) ofSetColor(192);
			ofDrawCircle(XCntr - 512 + BArr[i].X+40, BArr[i].Y+70, 10);
			if (Star_Clear[i] < 2) ofSetColor(192);
			ofDrawCircle(XCntr - 512 + BArr[i].X+70, BArr[i].Y+70, 10);
			if (Star_Clear[i] < 3) ofSetColor(192);
			ofDrawCircle(XCntr - 512 + BArr[i].X+100, BArr[i].Y+70, 10);
			// �ִܽð� �׸� �̸� ��� (Time)
			ofSetColor(64);
			myFont24.drawString("Time", XCntr - 512 + BArr[i].X+10, BArr[i].Y+120);
			// �ִܽð� ��� (�Ҽ��� 2�ڸ����� �����)
			ofSetColor(0);
			int Time = (int)(Time_Clear[i]);
			itoa(Time, TBuf, 10);
			int len = strlen(TBuf);
			TBuf[len] = '.';
			TBuf[len + 1] = '0'+(int)(Time_Clear[i] * 10) % 10;
			TBuf[len + 2] = '0'+(int)(Time_Clear[i] * 100) % 10;
			TBuf[len + 3] = '\0';
			myFont16.drawString(TBuf, XCntr - 512 + BArr[i].X+70, BArr[i].Y+160);
		}
		break;
	case 4:
		// ���ӿ��� ȭ�� : ��� ���
		ofSetBackgroundColor(255);
		// ���ӿ��� ���� �׸���
		ofSetColor(64, 0, 0);
		myFont60.drawString("Game Over!", XCntr - 250+2, 300+2);
		// ���ӿ��� ���� ��ü
		ofSetColor(255,0,0);
		myFont60.drawString("Game Over!", XCntr - 250, 300);

		// ��ư �׸���
		ofSetColor(128);
		ofDrawRectangle(XCntr - 300+5, 440+5, 260, 80);
		ofDrawRectangle(XCntr + 30+5, 440+5, 260, 80);
		// ��õ� ��ư ��ü
		ofSetColor(255, 224, 224);
		ofDrawRectangle(XCntr - 300, 440, 260, 80);
		// �޴��� ���ư��� ��ư ��ü
		ofSetColor(255, 128, 128);
		ofDrawRectangle(XCntr + 30, 440, 260, 80);
		// ��ư ��Ʈ�� ���
		ofSetColor(128);
		myFont16.drawString("Retry Stage (R)", XCntr - 260, 485);
		myFont16.drawString("Return To Menu (Q)", XCntr+60, 485);
		break;
	case 5:
		// ���� Ŭ���� ȭ�� : ��� ���
		ofSetBackgroundColor(255);
		// Ŭ���� ���� �׸���
		ofSetColor(0, 64, 64);
		myFont60.drawString("Stage Clear!", XCntr - 260+2, 240+2);
		// Ŭ���� ���� ��ü
		ofSetColor(0, 255, 255);
		myFont60.drawString("Stage Clear!", XCntr - 260, 240);
		
		// ���� ���
		if (Time_Clear[prevScreen - 10] == 0 || Time_Clear[prevScreen - 10] > Box.Time) // �ִܽð��� ��� �ִܽð� �迭�� ������ (Time_Clear�� 0�̸� ù Ŭ�����̹Ƿ� ������ ������)
			Time_Clear[prevScreen - 10] = Box.Time;
		if (Stage_Clear <= prevScreen - 10) // �������� Ŭ���� �� ����
			Stage_Clear = prevScreen + 1 - 10;
		// ���� ��� ���� : (���� ���� ���� * 25 + 100*�����ð� ����) * 1(��Ʈ �̻���) or *0.5(��Ʈ ����)
		score = (Box.Coin * 25 + 100 * (Limit_Clear[prevScreen - 10] - Box.Time) / Limit_Clear[prevScreen - 10]) / (2 - Box.hint);
		// ������ ȹ�� ���� ��� (>=75 :3��, 50~75 : 2��, 25~50 : 1��, <25 : 0��)
		if (score >= 75)
			rtemp = 3;
		else if (score >= 50)
			rtemp = 2;
		else if (score >= 25)
			rtemp = 1;
		else 
			rtemp = 0;

		if (Star_Clear[prevScreen - 10] < rtemp) // �ְ� ���� ���� ����
			Star_Clear[prevScreen - 10] = rtemp;
		update_file(); // save.txt�� ����� ������

		// ȹ�� ���� ���� ��� (ȹ�� : �����, ��ȹ�� : ȸ��)
		ofSetColor(255, 255, 0);
		if (rtemp < 1) ofSetColor(192);
		ofDrawCircle(XCntr - 160, 360, 50);
		if (rtemp < 2) ofSetColor(192);
		ofDrawCircle(XCntr , 360, 50);
		if (rtemp < 3) ofSetColor(192);
		ofDrawCircle(XCntr + 160, 360, 50);

		// ��ư ��� 
		if (prevScreen == 14) // ������ �������� (���� �������� ��ư ����)
		{
			// ��ư �׸���
			ofSetColor(128);
			ofDrawRectangle(XCntr - 135+5, 475+5, 260, 80);
			// ��ư ��ü
			ofSetColor(128, 255, 255);
			ofDrawRectangle(XCntr - 135, 475, 260, 80);
			// ��ư ��Ʈ��
			ofSetColor(128);
			myFont16.drawString("Return To Menu (Q)", XCntr - 105, 520);
		}
		else // 1~4�������� 
		{
			// ��ư �׸���
			ofSetColor(128);
			ofDrawRectangle(XCntr - 300+5, 475+5, 260, 80);
			ofDrawRectangle(XCntr + 30+5, 475+5, 260, 80);
			// ���� �������� ��ư ��ü
			ofSetColor(192, 255, 255);
			ofDrawRectangle(XCntr - 300, 475, 260, 80);
			// �޴��� ���ư��� ��ư ��ü
			ofSetColor(128, 255, 255);
			ofDrawRectangle(XCntr + 30, 475, 260, 80);
			// ��ư ��Ʈ��
			ofSetColor(128);
			myFont16.drawString("Next Stage (S)", XCntr - 245, 520);
			myFont16.drawString("Return To Menu (Q)", XCntr + 60, 520);
		}
		break;
	case 10: // Stage 1
	case 11: // Stage 2
	case 12: // Stage 3
	case 13: // Stage 4
	case 14: // Stage 5
		// ���� �÷��� ȭ��
		int Stage = Screen - 10; // ���� �������� ��

		double MXCntr = Box.baseX + Box.L * MArr[Stage].Col / 2; // �̷��� �߽� X��ǥ 
		double MYCntr = Box.baseY + Box.L * MArr[Stage].Row / 2; // �̷��� �߽� Y��ǥ
		double MXSize = Box.L * MArr[Stage].Col; // �̷��� ���� ũ�� 
		double MYSize = Box.L * MArr[Stage].Row; // �̷��� ���� ũ��
		double MScrdx = Box.baseX - Box.defaultX; // ù DrawMazeȣ��� ȭ��ũ��� ���� ȭ��ũ���� ������ (X��ǥ)
		double MScrdy = Box.baseY - Box.defaultY; // ù DrawMazeȣ��� ȭ��ũ��� ���� ȭ��ũ���� ������ (Y��ǥ)

		// ������
		ofSetBackgroundColor(255);
		// �������� ȸ�� ����â ���
		ofSetColor(224);
		ofDrawRectangle(XCntr + 250, 0, ofGetWidth() - (XCntr + 250), ofGetHeight());
		double Side = XCntr + 250; // ����â ���� X��ǥ

		Box.Time = ofGetElapsedTimef() - Box.StartTime; // ����ð� ����
		Matrix_Clear(Matrix); // ��ȯ�� ����� �ʱ�ȭ�� (Matrix = I)

		// �� Stage �̷� ��ȯ �ڵ� (��Ŀ� ��ȯ���� ���ϴ� ���·� ��ø��)
		if (Stage == 0)
		{
			// 1�������� : �ʴ� 4���� �ð�������� ȸ��
			Stack_RotateAt(Box.Time * 4, MXCntr, MYCntr); // �߽���ġ���� ȸ��
		}
		else if (Stage == 1)
		{
			// 2�������� : X�������� 45�� �ֱ�� ���ܺ��� + 45�� ����� X������ ������ 
			double prt = 22.5, Cycle = fmodl(Box.Time, prt * 2);
			double ratio = (prt - abs(Cycle - prt)) / prt;
			Stack_Shear(ratio, 0);
			Stack_Move(-MXSize*(ratio) / 2, 0);
			if (Box.Time >= prt*2)
			{
				Stack_Reflect(1, 0);
				Stack_Move((Box.baseX+MXSize/2)*2, 0);
			}
		}
		else if (Stage == 2)
		{
			// 3�������� : 10�� �ֱ�� �������� �ִ� 30���� ȸ���� (��ӿ)
			double prt = 2.5, Angle, MaxAngle = 30, MaxVelocity = 2 * MaxAngle / prt;
			double Cycle = fmodl(Box.Time, 4*prt);

			// MaxAngle = 0.5*AngluarAcceleration*(�ֱ�/4)^2 ���·� ������ (�֢עآټ�)
			if (Cycle <= prt) // �� ���� �̵�
				Angle = MaxVelocity * Cycle * (1 - Cycle / 2 / prt);
			else if (Cycle <= prt * 2) // �� ���� �̵�
			{
				Cycle = 2 * prt - Cycle;
				Angle = MaxVelocity * Cycle * (1 - Cycle / 2 / prt);
			}
			else if (Cycle <= prt * 3) // �� ���� �̵�
			{
				Cycle = Cycle - 2 * prt;
				Angle = -MaxVelocity * Cycle * (1 - Cycle / 2 / prt);
			}
			else // �� ���� �̵�
			{
				Cycle = 4 * prt - Cycle;
				Angle = -MaxVelocity * Cycle * (1 - Cycle / 2 / prt);
			}

			Stack_RotateAt(Angle, MXCntr, MYCntr); // ȸ����ȯ
			Stack_Move(-Box.L * 1, Box.L * 1.5); // ��ġ ����� ����
		}
		else if (Stage == 3)
		{
			// 4�������� : 4.5*4�ʸ� �ֱ�� 4.5�ʸ��� 90���� �ð� �ݴ�������� ȸ����
			double prt = 4.5, Angle;
			double Cycle = fmodl(Box.Time, 4 * prt);
			if (Cycle <= prt)
				Angle = 0;
			else if (Cycle <= prt * 2)
				Angle = -90;
			else if (Cycle <= prt * 3)
				Angle = -180;
			else
				Angle = -270;
			Stack_RotateAt(Angle, MXCntr, MYCntr); // ȸ����ȯ
		}
		else if (Stage == 4)
		{
			// 5�������� : �ʴ� 16���� �ð�������� ȸ�� -> ������ ���� ũ�⸦ 0.5��� ����� ���� ��ȯ (RUY ->RUY*0.5, RDY->RDY*0.5) -> ũ�� �� ��ġ ����
			double prt = 45 / 2;
			Stack_RotateAt(360 / prt * Box.Time, MXCntr, MYCntr); // ȸ�� ��ȯ
			Stack_Move((512 - XCntr)*1.4, 0); // ȭ�� ũ�⸦ ����� ��ġ�̵�
			Stack_Project(Stage,mapX(0),mapY(0), mapX(0), mapY(MArr[Stage].Row), mapX(MArr[Stage].Col), mapY(MArr[Stage].Row / 4), mapX(MArr[Stage].Col), mapY(MArr[Stage].Row * 3 / 4)); // ���� ��ȯ
			Stack_Scale(1.4, 1.4); // ũ�� ����
			Stack_Move(-MXCntr / 2 - (512 - XCntr) * 1, -MYCntr / 2); // ��ġ ����
		}

		Stack_Move((512 - XCntr) / 2,0); // �ʵ� ���� ��� ���� 
		// �� �����̺� Ŀ���� �ڵ� ����

		Matrix_Inverse(Matrix, Inverse); // ��ø�� ��ȯ����� ����� ���

		// Draw Maze (��Ʈ ���� DFS ������� ��Ʈ�� �����, ù ȣ��� ���� ȭ�� ũ�⸦ �������� �̷��� Orig, WOrig, WType�� �����)
		DrawMaze(Stage);

		// ���� ��ȯ�� �浹������ �����
		for (int k = 0; k < MArr[Stage].Orig.size(); k++) // Orig (��ȯ�� �浹����) ��ȸ
		{
			Block BlockT = MArr[Stage].Orig[k];
			for (int l = 0; l < 4; l++) // ���� ȭ�� ũ��� ������ (�����̵�)
			{
				BlockT.P[l].X += MScrdx; 
				BlockT.P[l].Y += MScrdy;
			}
			MArr[Stage].Trans[k] = Matrix_TransBlock(Matrix, BlockT); // ������ ���� ��ȯ��ķ� Trans (��ȯ�� �浹����) ���
		}
		int l = 0;
		for (int k = 0; k < MArr[Stage].WOrig.size(); k++) // WOrig (��ȯ�� ��-�� �浹����) ��ȸ
			if (MArr[Stage].Wtype[k] != 3) // �ӽÿ� Ÿ�� 3���� ����
			{
				MArr[Stage].WTest[l] = MArr[Stage].Wtype[k]; // WTest�� ���� Block�� Ÿ�� �״�� ���� (Wtype)

				Line LinT = MArr[Stage].WOrig[k];
				LinT.x1 += MScrdx; // ���� ȭ�� ũ��� ������ (�����̵�)
				LinT.y1 += MScrdy;
				LinT.x2 += MScrdx;
				LinT.y2 += MScrdy;
				MArr[Stage].Wall[l++] = Matrix_TransLine(Matrix, LinT); // ������ ���� ��ȯ��ķ� Wall (��ȯ�� ��-�� �浹����) ���
			}
		// Draw Water
		for (int k = 0; k < WaterNum[Stage]; k++) // ���ٱ� �迭 ��ȸ
		{
			double Timer = ofGetElapsedTimef() - MArr[Stage].Hole[k].StartTime + MArr[Stage].Hole[k].TotalTime; 
			double N = (Timer / MArr[Stage].Hole[k].TotalTime);
			double R = (N - (int)N) * MArr[Stage].Hole[k].TotalTime; // StartTime���� �����ϴ� �ֱⰡ TotalTime�� Ÿ�̸�
			if (R <= MArr[Stage].Hole[k].EnableTime) // ���ٱⰡ �ش� Ÿ�̸ӿ��� EnableTime ������ ������ ��� ����
				MArr[Stage].Hole[k].run = 1; // ���ٱ� ����
			else
				MArr[Stage].Hole[k].run = 0; // ���ٱ� �ߴ�
		}

		DrawWater(Stage); // ���ٱ� �� ��� ��� ����� �׸�

		// �÷��̾ ���ٱ� �� ��Ͽ� ��Ҵ����� üũ
		for (int l = 0; l < MArr[Stage].Hole.size(); l++) // ���ٱ� �迭 ��ȸ
		{
			vector<Block>&BArr = MArr[Stage].Hole[l].wblock; // wblock : ��-�÷��̾� �浹���� �迭
			for (int k = 0; k < MArr[Stage].Hole[l].size - 1; k++) // ���� ���ٱ� - �÷��̾� �浹������ ��ȸ
			{
				if (MArr[Stage].Hole[l].run == 0) continue; // ���ٱⰡ �������� �ƴϸ� pass
				if (CheckCollide(BArr[k])) // ���� block�� �÷��̾ �浹�ߴ��� check
				{
					if (MArr[Stage].Hole[l].type == 0) // ���� �ڽ��� ���ٱ� -> ��ȭ ����
					{
						Box.SlowStart = ofGetElapsedTimef();
						Box.Slowflag = 1;
					}
					else // ���� �ڽ��� ���ٱ� -> ���ӿ���
					{
						ofClear(255);
						Screen = 4;
					}
				}
			}
		}

		// Draw Character
		Box.TLoc = Matrix_Trans(Matrix, Box.Loc); // ��ȯ�� �÷��̾� ��ġ ���

		// ��ȯ�� ��ġ�� �������� �����¿�*Box.speed ��ŭ �̵� -> ����ķ� ��ȯ�� �̵��Ÿ��� ������
		Box.Move.P[0] = { Box.TLoc.X,Box.TLoc.Y - Box.speed }; // U
		Box.Move.P[1] = { Box.TLoc.X,Box.TLoc.Y + Box.speed }; // D
		Box.Move.P[2] = { Box.TLoc.X - Box.speed,Box.TLoc.Y }; // L
		Box.Move.P[3] = { Box.TLoc.X + Box.speed,Box.TLoc.Y }; // R
		Box.Move = Matrix_TransBlock(Inverse, Box.Move); // �Ϲ� �̵� �Ÿ� ����
		Box.SlowMove.P[0] = { Box.TLoc.X,Box.TLoc.Y - Box.speed/2 }; // U
		Box.SlowMove.P[1] = { Box.TLoc.X,Box.TLoc.Y + Box.speed/2 }; // D
		Box.SlowMove.P[2] = { Box.TLoc.X - Box.speed/2,Box.TLoc.Y }; // L
		Box.SlowMove.P[3] = { Box.TLoc.X + Box.speed/2,Box.TLoc.Y }; // R
		Box.SlowMove = Matrix_TransBlock(Inverse, Box.SlowMove); // ��ȭ �̵� �Ÿ� ����
		for (int k = 0; k < 4; k++) // �̵��� ��ġ - �̵��� ��ġ�� �̵��Ÿ��� ������
		{
			Box.Move.P[k].X -= Box.Loc.X;
			Box.Move.P[k].Y -= Box.Loc.Y;
			Box.SlowMove.P[k].X -= Box.Loc.X;
			Box.SlowMove.P[k].Y -= Box.Loc.Y;
		}

		// �÷��̾� ĳ���� : ��ȫ�� �ڽ�
		ofSetColor(255, 0, 255);
		ofDrawRectangle(Box.TLoc.X - Box.size / 2, Box.TLoc.Y - Box.size / 2, Box.size, Box.size);

		// Draw Goal
		Point TDest = Matrix_Trans(Matrix, Box.Dest);
		int DestR = 15;
		ofSetColor(0, 255, 0);
		ofDrawCircle(TDest.X, TDest.Y, DestR);

		// Draw Coin
		int CoinR = 10;
		ofSetColor(240, 240, 0);
		for (int i = 0; i < 3; i++)
		{
			Point Ptemp = Matrix_Trans(Matrix, Box.CoinDest[i].X, Box.CoinDest[i].Y);
			if (Box.GetCoin[i] == 0) // ������ ���� ������� �׸�
				ofDrawCircle(Ptemp.X, Ptemp.Y, CoinR);
			if (Box.GetCoin[i] == 0 && CheckCircle(Ptemp.X, Ptemp.Y, CoinR)) // ������ �÷��̾ ������� �ش� ������ ���������� ó��
			{
				Box.GetCoin[i] = 1;
				Box.Coin++;
			}
		}

		// Draw Text
		// �������� ��
		ofSetColor(0, 0, 255);
		char sname[] = { "Stage 10" };
		if (Stage < 9)
		{
			sname[7] = '\0';
			sname[6] = '1' + Stage;
		}
		myFont20.drawString(sname, Side + 20, 80);

		// �ð� ����
		ofSetColor(128);
		myFont20.drawString("Time Limit", Side + 20, 140);
		myFont20.drawString(to_string(Limit_Clear[Stage] - Box.Time), Side + 20, 175);

		// ��Ʈ ��������
		if (Box.hint == 1)
		{
			ofSetColor(256, 128, 0);
			myFont20.drawString("Hint : 1/1", Side + 20, 230);
			myFont20.drawString("Show Hint (H)", Side + 20, 260);
		}
		else
		{
			ofSetColor(128);
			myFont20.drawString("Hint : 0/1", Side + 20, 230);
			if (Box.hintTimer > 0) // ��Ʈ ���� �����ð��� ��Ȳ�� ������ �ٷ� ǥ����
			{
				ofSetColor(256, 128, 0);
				ofDrawRectangle(Side + 20, 240, (210 - Box.hintTimer * 21), 30);
			}
		}

		// ���� ���� ����
		ofSetColor(192, 192, 0);
		char CoinStr[] = { "Coin : 0/3" };
		CoinStr[7] = '0' + Box.Coin;
		myFont20.drawString(CoinStr, Side + 20, 310);
		ofSetColor(255, 255, 0);
		if (Box.Coin < 1) ofSetColor(192);
		ofDrawCircle(Side + 35, 330, 15);
		if (Box.Coin < 2) ofSetColor(192);
		ofDrawCircle(Side + 70, 330, 15);
		if (Box.Coin < 3) ofSetColor(192);
		ofDrawCircle(Side + 105, 330, 15);

		// ��ȭ �����ϰ�� �ʷϻ� ������ �ٷ� �����ð� ǥ�� 
		if (Box.Slowflag == 1)
		{
			Box.SlowTimer = 5 - (ofGetElapsedTimef() - Box.SlowStart);
			if (Box.SlowTimer > 0)
			{
				ofSetColor(0, 255, 128);
				ofDrawRectangle(Side + 20, 370, (Box.SlowTimer * 42), 30);
				ofSetColor(0);
				myFont16.drawString("Slow Down", Side + 25, 390);
			}
			else
				Box.Slowflag = 0; // 5�ʰ� ������ ��ȭ ����
		}

		// Clear
		if (CheckCircle(TDest.X, TDest.Y, DestR)) // �������� ������ �¸�
		{
			ofClear(255);
			Screen = 5;
		}
		// Fail
		if (Limit_Clear[Stage] - Box.Time < 0) // �ð����� ������ ���ӿ���
		{
			ofClear(255);
			Screen = 4;
		}
		break;
	}
} 

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (Screen) // �� ȭ�鿡���� Ű���� �ν� �ڵ� ����
	{
	case 1: // ����ȭ��
		if (key == 's' || key == 'S') { // �޴�ȭ������ �̵�
			ofClear(224);
			Screen = 3;
		}
		else if (key == 'h' || key == 'H') // ���Ӽ������� �̵�
		{
			ofClear(255, 255, 255);
			Screen = 2;
		}
		break;
	case 2: // ���Ӽ��� ȭ��
		if (key == 's' || key == 'S') { // �޴�ȭ������ �̵�
			ofClear(224);
			Screen = 3;
		}
		else if (key == 'q' || key == 'Q') // ����ȭ������ �̵�
		{
			ofClear(255, 255, 255);
			Screen = 1;
		}
		break;
	case 3: // �������� ����â (�޴�ȭ��)
		if (key == 'q' || key == 'Q') // ����ȭ������ �̵�
		{
			ofClear(255, 255, 255);
			Screen = 1;
		}
		// 1~5�������� ���� 1~5��Ű�� �����ϴ� �Լ� (�ش� ���������� �رݵ� ��츸 ����, ���۽� �ش� ���������� �ʱ�ȭ��)
		else if (key == '1' && Stage_Clear >= 0) // 1�������� ����
		{
			ofClear(255);
			Screen = 10 + 0;
			prevScreen = 10 + 0;
			StageReset(0);
		}
		else if (key == '2' && Stage_Clear >= 1) // 2�������� ����
		{
			ofClear(255);
			Screen = 10 + 1;
			prevScreen = 10 + 1;
			StageReset(1);
		}
		else if (key == '3' && Stage_Clear >= 2) // 3�������� ����
		{
			ofClear(255);
			Screen = 10 + 2;
			prevScreen = 10 + 2;
			StageReset(2);
		}
		else if (key == '4' && Stage_Clear >= 3) // 4�������� ����
		{
			ofClear(255);
			Screen = 10 + 3;
			prevScreen = 10 + 3;
			StageReset(3);
		}
		else if (key == '5' && Stage_Clear >= 4) // 5�������� ����
		{
			ofClear(255);
			Screen = 10 + 4;
			prevScreen = 10 + 4;
			StageReset(4);
		}

		break;
	case 4: // ���ӿ��� ȭ��
		if (key == 'q' || key == 'Q') // �޴�ȭ������ �̵�
		{
			ofClear(224);
			Screen = 3;
		}
		else if (key == 'r' || key == 'R') // �����
		{
			ofClear(255, 255, 255);
			// ���� ȭ�鿡 ����� ���������� �ٽ� �÷�����
			Screen = prevScreen;
			StageReset(prevScreen-10);
		}
		break;
	case 5: // Ŭ���� ȭ��
		if (key == 'q' || key == 'Q') // �޴�ȭ������ �̵�
		{
			ofClear(224);
			Screen = 3;
		}
		else if (prevScreen != 14 && (key == 's' || key == 'S')) // ���� ����������
		{
			ofClear(255, 255, 255);
			// ���� ���������� �ʱ�ȭ�ϰ� �÷�����
			Screen = prevScreen+1;
			StageReset(prevScreen+1 - 10);
			prevScreen += 1;
		}
		break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		// 1~5���������� Ű�ν� ó�� �Լ�
		double dx, dy, Tdx, Tdy; // ���� ��ȯ�� x, y��ǥ ��ȭ��, ��ȯ�� x, y��ǥ ��ȭ��
		if (key == 'q' || key == 'Q') // �޴�ȭ������ �̵�
		{
			ofClear(224);
			Screen = 3;
		}
		if (Box.hint == 1 &&(key == 'h' || key == 'H')) // ��Ʈ ���
		{
			Box.hint = 0;
			Box.hintTimer = 10;
			Box.hintStartTime = ofGetElapsedTimef(); // ��Ʈ ���۽ð��� ����ð����� ����
		}
		if (key == OF_KEY_UP) // ȭ��ǥ ��Ű
		{
			if (Box.Slowflag == 0) // �Ϲ�
			{
				Tdy = -Box.speed;
				dx = Box.Move.P[0].X;
				dy = Box.Move.P[0].Y;
			}
			else // ��ȭ
			{
				Tdy = -Box.slow_speed;
				dx = Box.SlowMove.P[0].X;
				dy = Box.SlowMove.P[0].Y;
			}

			// ������ �̵� (CheckMaze�� �浹üũ ��, �����ϸ� ��ȯ�� ��ǥ�� ������ ������ �÷��̾� ĳ���� �̵���Ŵ)
			Box.TLoc.Y += Tdy; 
			if(CheckMaze(Screen-10)) // �浹üũ�� ��ȯ�� ��ǥ�� ����������
				Box.TLoc.Y -= Tdy;
			else {
				Box.Loc.X += dx;
				Box.Loc.Y += dy;
			}
		}
		if (key == OF_KEY_DOWN) // ȭ��ǥ ��Ű
		{
			if (Box.Slowflag == 0)  // �Ϲ�
			{
				Tdy = Box.speed;
				dx = Box.Move.P[1].X;
				dy = Box.Move.P[1].Y;
			}
			else // ��ȭ
			{
				Tdy = Box.slow_speed;
				dx = Box.SlowMove.P[1].X;
				dy = Box.SlowMove.P[1].Y;
			}

			// �Ʒ����� �̵� (CheckMaze�� �浹üũ ��, �����ϸ� ��ȯ�� ��ǥ�� ������ ������ �÷��̾� ĳ���� �̵���Ŵ)
			Box.TLoc.Y += Tdy;
			if (CheckMaze(Screen - 10)) // �浹üũ�� ��ȯ�� ��ǥ�� ����������
				Box.TLoc.Y -= Tdy;
			else {
				Box.Loc.X += dx;
				Box.Loc.Y += dy;
			}
		}
		if (key == OF_KEY_LEFT) // ȭ��ǥ ��Ű
		{
			if (Box.Slowflag == 0)  // �Ϲ�
			{
				Tdx = -Box.speed;
				dx = Box.Move.P[2].X;
				dy = Box.Move.P[2].Y;
			}
			else // ��ȭ
			{
				Tdx = -Box.slow_speed;
				dx = Box.SlowMove.P[2].X;
				dy = Box.SlowMove.P[2].Y;
			}

			// ���ʹ��� �̵� (CheckMaze�� �浹üũ ��, �����ϸ� ��ȯ�� ��ǥ�� ������ ������ �÷��̾� ĳ���� �̵���Ŵ)
			Box.TLoc.X += Tdx;
			if (CheckMaze(Screen - 10)) // �浹üũ�� ��ȯ�� ��ǥ�� ����������
				Box.TLoc.X -= Tdx;
			else {
				Box.Loc.X += dx;
				Box.Loc.Y += dy;
			}
		}
		if (key == OF_KEY_RIGHT) // ȭ��ǥ ��Ű
		{
			if (Box.Slowflag == 0)  // �Ϲ�
			{
				Tdx = Box.speed;
				dx = Box.Move.P[3].X;
				dy = Box.Move.P[3].Y;
			}
			else // ��ȭ
			{
				Tdx = Box.slow_speed;
				dx = Box.SlowMove.P[3].X;
				dy = Box.SlowMove.P[3].Y;
			}

			// �����ʹ��� �̵� (CheckMaze�� �浹üũ ��, �����ϸ� ��ȯ�� ��ǥ�� ������ ������ �÷��̾� ĳ���� �̵���Ŵ)
			Box.TLoc.X += Tdx;
			if (CheckMaze(Screen - 10)) // �浹üũ�� ��ȯ�� ��ǥ�� ����������
				Box.TLoc.X -= Tdx;
			else {
				Box.Loc.X += dx;
				Box.Loc.Y += dy;
			}
		}
		break;
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	int XCntr = ofGetWidth() / 2; // ȭ���� �߰� ��ġ (X��ǥ)
	switch (Screen) // �� ȭ�鿡���� ���콺 Ŭ�� �ν� �ڵ� ����
	{
	case 1: // ����ȭ��
		if (x >= XCntr-130 && x <= XCntr+130 && y >= 440 && y <= 490) { // �޴�ȭ������ �̵�
			ofClear(224);
			Screen = 3;
		}
		else if (x >= XCntr - 130 && x <= XCntr + 130 && y >= 520 && y <= 570) // ���Ӽ������� �̵�
		{
			ofClear(255, 255, 255);
			Screen = 2;
		}
		break;
	case 2: // ���Ӽ���
		if (x >= XCntr - 130 && x <= XCntr + 130 && y >= 660 && y <= 710) { // �޴�ȭ������ �̵�
			ofClear(224);
			Screen = 3;
		}
		break;
	case 3: // �������� ����â (�޴�ȭ��)
		for (int i = 0; i < Stage_Num; i++)
		{
			if (Stage_Clear >= i && x >= XCntr - 512 + BArr[i].X && x <= XCntr - 512 + BArr[i].X+140 && y >= BArr[i].Y && y <= BArr[i].Y + 180) { // �ش� ���������� �رݵǾ��ִٸ�, �� ���������� �̵��� 
				ofClear(255);
				Screen = 10+i;
				prevScreen = 10 + i; 
				StageReset(i); // �������� �ʱ�ȭ
			}
		}
		break;
	case 4: // ���ӿ��� ȭ��
		if (x >= XCntr - 300 && x <= XCntr - 40 && y >= 440 && y <= 520) { // ��õ�
			ofClear(255);
			// ���� ȭ������ ���ư��� �ش� ���������� �ٽ� �÷�����
			Screen = prevScreen;
			StageReset(prevScreen - 10);
		}
		else if (x >= XCntr - 30 && x <= XCntr + 230 && y >= 440 && y <= 520) { // �޴�ȭ������ �̵�
			ofClear(224);
			Screen = 3;
		}
		break;
	case 5: // Ŭ���� ȭ��
		if (prevScreen == 14) // ������ �������� Ŭ�����
		{
			if (x >= XCntr - 135 && x <= XCntr + 125 && y >= 475 && y <= 555) { // �޴�ȭ������ �̵�
				ofClear(224);
				Screen = 3;
			}
		}
		else // 1~4�������� Ŭ�����
		{
			if (x >= XCntr - 300 && x <= XCntr - 40 && y >= 475 && y <= 555) { // ���� ��������
				ofClear(255);
				// ���� ���������� �÷�����
				Screen = prevScreen + 1;
				StageReset(prevScreen + 1 - 10);
				prevScreen += 1;
			}
			else if (x >= XCntr - 30 && x <= XCntr + 230 && y >= 475 && y <= 555) { // �޴�ȭ������ �̵�
				ofClear(224);
				Screen = 3;
			}
		}
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void SetCoinLoc(int Num, int Limit) // ������ ��ġ�� �缳����
{
	for (int i = 0; i < 3; i++) // �� ���� ���� : 3��
	{
		while (1)
		{
			Box.GetCoin[i] = 0; // �Ծ����� ���θ� �ʱ�ȭ
			Box.CoinDest[i].Y = (int)ofRandom(0, MArr[Num].Row); // X,Y��ǥ �̷� ������ ������ġ�� ����
			Box.CoinDest[i].X = (int)ofRandom(0, MArr[Num].Col);
			if (Box.CoinDest[i].Y + Box.CoinDest[i].X > Limit || Box.CoinDest[i].Y + Box.CoinDest[i].X < MArr[Num].Col + MArr[Num].Row - 2 - Limit) // ������ ��ġ�� ���� ������ �ִ°�� Ż�� (Limit ���_
				break;
		}
	}
}

void SetStageSetting(int Num, double Zoom)  // ���������� ������ �ʱ�ȭ��
{
	if (MArr[Num].Orig.size() == 0) // ó�� �ش� ���������� ������ ���
	{
		ScrX[Num] = ofGetWidth(); // DrawMaze ù ȣ��� ����� ȭ�� ũ�Ⱚ�� ���� ������
		ScrY[Num] = ofGetHeight();
	}
	Box.zoom = Zoom;
	Box.speed = 3 * Box.zoom;
	Box.slow_speed = Box.speed / 2;
	Box.baseX = ofGetWidth() / 2 - 125 - (MArr[Num].Row) * Length * Box.zoom / 2;
	Box.baseY = ofGetHeight() / 2 - (MArr[Num].Col)*Length *Box.zoom / 2;
	Box.defaultX = ScrX[Num] / 2 - 125 - (MArr[Num].Row) * Length * Box.zoom / 2;
	Box.defaultY = ScrY[Num] / 2 - (MArr[Num].Col) * Length * Box.zoom / 2;
	Box.L = Length * Box.zoom;
	Box.Loc.X = Box.baseX + Box.L * (0.5);
	Box.Loc.Y = Box.baseY + Box.L * (0.5);
	Box.TLoc = Box.Loc;
	Box.Dest.X = Box.baseX + Box.L * (0.5 + MArr[Num].Col - 1);
	Box.Dest.Y = Box.baseY + Box.L * (0.5 + MArr[Num].Row - 1);

	for (int i = 0; i < 3; i++) // ������ ��ǥ�� index���� ȭ����ǥ�� ��ȯ��
	{
		Box.CoinDest[i].X = Box.baseX + Box.L * (Box.CoinDest[i].X + 0.5);
		Box.CoinDest[i].Y = Box.baseY + Box.L * (Box.CoinDest[i].Y + 0.5);
	}
}

void StageReset(int Num) // ���������� �ʱ�ȭ��
{
	// ��� �������� ���� ����
	Box.Time = 0.;
	Box.Coin = 0.;
	Box.SlowTimer = 0.;
	Box.size = 15;
	Box.hint = 1;
	Box.StartTime = ofGetElapsedTimef(); // ���۽ð� ����
	Box.Slowflag = 0;
	Matrix_Clear(Matrix); // ��ȯ�� ����� �ʱ�ȭ�� (Matrix = I)
	Box.hintTimer = 0;
	for (int i = 0; i < 4; i++)
	{
		Box.Move.P[i] = { 0,0 };
		Box.SlowMove.P[i] = { 0,0 };
	}
	SetCoinLoc(Num, 3); // ������ġ �ʱ�ȭ
	
	int loc = 0; // �ӽú���
	// Stage�� �ʱ�ȭ ����
	switch (Num)
	{
	case 0: // Stage 1
		SetStageSetting(Num, 2); // �������� ���� �ʱ�ȭ
		
		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 1�������� : 2���� ���ٱ⸦ �̷� ������ ������ ���� ������, 6�� �ֱ�� 4.5�ʾ� ���ÿ� �߻�
			MArr[Num].Hole[i].StartTime = Box.StartTime;
			MArr[Num].Hole[i].EnableTime = 4.5;
			MArr[Num].Hole[i].TotalTime = 6;
			MArr[Num].Hole[i].t = 7.5 * Box.zoom;
			MArr[Num].Hole[i].run = 1;
			MArr[Num].Hole[i].type = 0;
			MArr[Num].Hole[i].Dot = { Box.baseX + Box.L * (double)(0.5 + (int)ofRandom(0, MArr[Num].Col)) 
				,Box.baseY + Box.L * (double)(0.5 + (int)ofRandom(0, MArr[Num].Row)) };
		}
		break;
	case 1: // Stage 2
		SetStageSetting(Num, 1.8); // �������� ���� �ʱ�ȭ

		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 2�������� : 2���� ���ٱ�� ����� ���� �����¿� �� ������ ������ ���� ������, 8�� �ֱ�� 2�ʾ� 1���� �����ư��� �߻�
			MArr[Num].Hole[i].StartTime = Box.StartTime + i * 2;
			MArr[Num].Hole[i].EnableTime = 2;
			MArr[Num].Hole[i].TotalTime = 8;
			MArr[Num].Hole[i].t = 7.5 * Box.zoom;
			MArr[Num].Hole[i].run = 0;
			if (i % 2 == 0)
				MArr[Num].Hole[i].type = 0;
			else
				MArr[Num].Hole[i].type = 1;
		}

		MArr[Num].Hole[0].Dot = { mapX(0.5),mapY(0.5 + (int)ofRandom(1, MArr[Num].Row - 1))}; // ��
		MArr[Num].Hole[1].Dot = { mapX(0.5 + (int)ofRandom(1, MArr[Num].Col - 1)),mapY(0.5)}; // ��
		MArr[Num].Hole[2].Dot = { mapX(0.5 + MArr[Num].Col - 1),mapY(0.5 + (int)ofRandom(1, MArr[Num].Row - 1)) }; // ��
		MArr[Num].Hole[3].Dot = { mapX(0.5 + (int)ofRandom(1, MArr[Num].Col - 1)),mapY(0.5 + MArr[Num].Row - 1) }; // ��
		break;
	case 2: // Stage 3
		SetStageSetting(Num, 2); // �������� ���� �ʱ�ȭ

		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 3�������� : 10���� ���ٱ⸦ �� ���ٿ� ����, 5�� �ֱ�� 0.5�ʾ� ���ʺ��� �����ʱ��� ���ʴ�� 1���� �߻�
			MArr[Num].Hole[i].StartTime = Box.StartTime + 0.5 * i;
			MArr[Num].Hole[i].EnableTime = 0.5;
			MArr[Num].Hole[i].TotalTime = 5.0;
			MArr[Num].Hole[i].t = 7.5 * Box.zoom;
			MArr[Num].Hole[i].run = 0;
			MArr[Num].Hole[i].type = 0;
			MArr[Num].Hole[i].Dot = { mapX(1.5 + i),mapY(0.5) };
		}
		break;
	case 3: // Stage 4
		SetStageSetting(Num, 1.8); // �������� ���� �ʱ�ȭ

		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 4�������� : 12���� ����� �����¿� �� ���ٿ� 3���� 2ĭ�� ������ �ΰ� ���� (���� ������ġ 0~2�� ����), 4.5*4�� �ֱ�� 4.25�ʾ� �� ������ �����ư��� �߻�
			MArr[Num].Hole[i].StartTime = Box.StartTime + 4.5 * (i / 3);
			MArr[Num].Hole[i].EnableTime = 4.25;
			MArr[Num].Hole[i].TotalTime = 4.5*4;
			MArr[Num].Hole[i].t = 7.5 * Box.zoom;
			MArr[Num].Hole[i].run = 0;
			MArr[Num].Hole[i].type = 1;
			if (i % 3 == 0)
				loc = (int)ofRandom(0, 3); // ���� ������ġ ����ȭ (0~2)
			int q = i / 3, r = i % 3;
			if (q == 0)
				MArr[Num].Hole[i].Dot = { mapX(1.5 + loc + r * 3), mapY(0.2) }; // �� ����
			else if (q == 1)
				MArr[Num].Hole[i].Dot = { mapX(0.8 + MArr[Num].Col - 1), mapY(1.5 + loc + r * 3) }; // �� ������ ��
			else if (q == 2)
				MArr[Num].Hole[i].Dot = { mapX(1.5 + loc + r * 3), mapY(0.8 + MArr[Num].Row - 1) }; // �� �Ʒ��� 
			else if (q == 3)
				MArr[Num].Hole[i].Dot = { mapX(0.2), mapY(1.5 + loc + r * 3) }; // �� ���� ��
		}
		break;
	case 4: // Stage 5
		SetStageSetting(Num, 2.2); // �������� ���� �ʱ�ȭ

		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 5�������� : 4���� ���ٱ�� 4���� ����� �ʵ带 �� ��и鸶�� 1���� ����, 16�� �ֱ�� 4.5�� �� ��и��� ���ٱ�� ����� �߻�
			MArr[Num].Hole[i].StartTime = Box.StartTime + 4 * (i / 2);
			MArr[Num].Hole[i].EnableTime = 4.5;
			MArr[Num].Hole[i].TotalTime = 16;
			MArr[Num].Hole[i].t = 7.5 * Box.zoom;
			MArr[Num].Hole[i].run = 0;
			if (i % 2 == 0)
				MArr[Num].Hole[i].type = 0;
			else
				MArr[Num].Hole[i].type = 1;

			int rx = (int)ofRandom(0, 4), ry = (int)ofRandom(0, 4); // ��и� ���� ���� ��ġ�� ����
			int q = i / 2;
			if (q == 0)
			{
				if (rx == 0 && ry == 0) // 2��и��� ������ ��ġ(0,0)�� (1,1)�� �Ű� ���ۺ��� �й��ϴ� ��Ȳ ������
					rx = 1, ry = 1;
				MArr[Num].Hole[i].Dot = { mapX(0.5 + rx), mapY(0.5 + ry) }; // 2��и�
			}
			else if (q == 1)
				MArr[Num].Hole[i].Dot = { mapX(4.5 + rx), mapY(0.5 + ry) }; // 1��и�
			else if (q == 2)
				MArr[Num].Hole[i].Dot = { mapX(4.5 + rx), mapY(4.5 + ry) }; // 3��и�
			else if (q == 3)
				MArr[Num].Hole[i].Dot = { mapX(0.5 + rx), mapY(4.5 + ry) }; // 4��и�

			if (i % 2 == 1 && (MArr[Num].Hole[i].Dot.X == MArr[Num].Hole[i-1].Dot.X) && (MArr[Num].Hole[i].Dot.Y == MArr[Num].Hole[i - 1].Dot.Y)) // ��и��� ���� ��ĥ��� 
			{
				if (ry <= 2) // (y+1)%4 ������ ������
					MArr[Num].Hole[i].Dot.Y += mapY(1) - mapY(0);
				else 
					MArr[Num].Hole[i].Dot.Y -= mapY(3) - mapY(0);
			}
		}
		break;
	}
}

void update_file() // ���̺� �����͸� ���Ϸ� ����ϴ� �Լ�
{
	// bin/data ������ save.txt���Ͽ� Ŭ������ �������� ��, ���������� �ִ� ���ΰ���, �ִܽð��� ��ϵ�
	char fname[] = "save.txt";
	fopen_s(&fptr, ofToDataPath(fname).c_str(), "w");
	fprintf(fptr, "%d\n", Stage_Clear); // total clear Stage
	for (int i = 0; i < Stage_Num; i++)
		fprintf(fptr, "%d %lf\n", Star_Clear[i], Time_Clear[i]); // Star, Time
	fclose(fptr);
}

void Ensnare() // ��ȭ ���¸� �����ϴ� �Լ�
{
	Box.SlowStart = ofGetElapsedTimef(); // ����ð����� ��ȭ����
	Box.Slowflag = 1;
}
