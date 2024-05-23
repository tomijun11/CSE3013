#include "ofApp.h"
#include "Matrix.h"
#include "WaterFall.h"
#include "Maze.h"
#include "Calc.h"
#include <iostream>
#include <cmath>
using namespace std;

Node* Mtop = NULL; // DFS용 스택의 Top
#define Length 30.0 // 미로 한 칸의 기본 크기
FILE* fptr; // 세이브 파일 포인터
#define Stage_Num 5 // 총 스테이지 수
MData MArr[Stage_Num]; // 미로 데이터를 저장하는 배열
vector<Line> Path; // 힌트 경로를 저장하는 배열
Player Box; // 플레이어 데이터 저장
int Stage_Clear, Star_Clear[Stage_Num], Screen=1, prevScreen; // 각각 클리어 한 스테이지 수, 각 스테이지의 최고 코인 갯수, 현재 화면 번호, 이전 화면 번호
double Time_Clear[Stage_Num], Limit_Clear[Stage_Num] = {60.0,90.0,60.0,90.0,45.0}; // 각각 클리어한 최단시간, 스테이지별 시간제한
ofTrueTypeFont myFont24, myFont16, myFont12, myFont48, myFont60, myFont36, myFont20; // 폰트용 변수
Button BArr[Stage_Num]; // 버튼의 정보를 저장하는 배열
double Matrix[3][3]; // 변환용 행렬
double Inverse[3][3] = { {1,0,0},{0,1,0},{0,0,1} }; // 역변환용 행렬
int WaterNum[Stage_Num] = { 2,4,10,12,8 }; // 스테이지별 물줄기 갯수
double ScrX[Stage_Num], ScrY[Stage_Num]; // DrawMaze 첫 호출시 화면 크기 (각각 가로, 세로크기)

//--------------------------------------------------------------
void ofApp::setup() {

	// 이미지 로드
	Title.load("title.png");
	StageTitle.load("stage.png");
	tipTitle.load("tip.png");
	tip.load("tip2.png");

	ofSetWindowTitle("Maze Runner"); // 윈도우 창 이름 설정
	ofSetFrameRate(30); // 초당 프레임 수 설정
	ofBackground(255, 255, 255);
	
	// 윈도우 창 위치 설정
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);
	
	// 스테이지 버튼 위치 및 이름 설정
	char buf[Stage_Num]; 
	for (int i = 0; i < Stage_Num; i++)
	{
		strcpy_s(BArr[i].name, "Stage ");
		strcat_s(BArr[i].name, itoa(i + 1, buf, 10));
		BArr[i].X = 120 + 160 * (i % 5);
		BArr[i].Y = 320;
	}

	// 폰트 로드
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
	// save.txt 없는경우 파일을 생성하고 재로드
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
	char fname[50] = { "maze0.maz" }; // fname[4]만 바뀜
	for (int i = 0; i < Stage_Num; i++) // 각 스테이지의 미로 파일을 로드해서 MArr[i]에 저장함
	{
		fname[4] = '0' + i;
		fopen_s(&fmaze, ofToDataPath(fname).c_str(), "r"); // .maz파일 로드
		
		char templine[100] = { 0 };
		fgets(templine, 100, fmaze); // 1줄을 불러옴
		templine[strlen(templine) - 1] = '\0'; 

		MArr[i].Col = (strlen(templine) - 1) / 2; // .maz의 한 줄의 길이는 2*width+1 (\0제외) -> width를 역산함
		MArr[i].Row = 0;
		while (1) // feof 도달시까지 한줄씩 읽으면서 Row를 증가 -> .maz의 줄의 총 갯수가 저장됨
		{
			fgets(templine, 100, fmaze);
			templine[strlen(templine) - 1] = '\0';
		
			if (feof(fmaze))
				break;
			MArr[i].Row++;
		}
		if (strlen(templine) > 1)
			MArr[i].Row++;
		MArr[i].Row /= 2; // .maz의 줄의 갯수는 2*height+1 (eof제외) -> height를 역산함

		// Maze 데이터 할당
		int j, k;
		MArr[i].Maze = (Room**)malloc(sizeof(Room*) * MArr[i].Row); 
		for (j = 0; j < MArr[i].Row; j++)
			MArr[i].Maze[j] = (Room*)malloc(sizeof(Room) * MArr[i].Col);

		// Maze 데이터 초기화
		for (k = 0; k < MArr[i].Row; k++) 
			for (j = 0; j < MArr[i].Col; j++)
			{
				MArr[i].Maze[k][j].visited = 0;
				MArr[i].Maze[k][j].r = 1;
				MArr[i].Maze[k][j].d = 1;
			}

		fseek(fmaze, 0, 0); // 파일 포인터를 원점으로 되돌림
		fgets(templine, 100, fmaze); // 맨 윗줄 넘기기
		for (k = 0; k < MArr[i].Row; k++) // 한번에 2줄씩 읽음, 윗줄은 각 방의 오른쪽 벽(2*j+2), 아래줄은 각 방의 아래쪽 벽(2*j+1)의 정보를 담고있음 -> MArr[i].Maze에 값 갱신
		{
			char rline[100] = { 0 }; // 윗줄
			char dline[100] = { 0 }; // 아랫줄
			fgets(rline, 100, fmaze); 
			fgets(dline, 100, fmaze);

			for (j = 0; j < MArr[i].Col; j++) // 각 방의 값 갱신 (Col번 반복)
			{
				// fill rwall
				if (j < MArr[i].Col - 1) // 맨 오른쪽방 : 오른쪽 벽은 항상 벽이 있음
				{
					if (rline[2 * j + 2] == ' ')
						MArr[i].Maze[k][j].r = 0;
				}

				// fill dwall
				if (k < MArr[i].Row - 1) // 미로의 맨 아랫줄 : 아래쪽 벽은 항상 벽이 있음
				{
					if (dline[2 * j + 1] == ' ')
						MArr[i].Maze[k][j].d = 0;
				}
			}
		}
		fclose(fmaze);

		// 미로의 물줄기 정보 초기화
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
	/* Screen : 화면 출력 모드 설정
		1 : 시작화면
		2 : 게임설명
		3 : 메뉴화면
		4 : 게임오버
		5 : 승리화면
		10~14 : 해당 스테이지 화면
	*/
	int XCntr = ofGetWidth() / 2, YCntr = ofGetHeight(), i, j, rtemp; // XCntr : 화면의 중점좌표 (XCntr : X좌표, YCntr : Y좌표), 나머지는 임시변수
	double score; // 점수 계산용 임시변수
	switch (Screen) // 각 화면 출력시 실행할 코드
	{
	case 1:
		// 시작화면 : 흰색 배경
		ofSetBackgroundColor(255, 255, 255);
		// 제목 사진 출력
		ofSetColor(255);
		Title.draw(XCntr - 400, 120);
		// 버튼의 그림자 
		ofSetColor(0);
		ofDrawRectRounded(XCntr - 130+5, 440+5, 260, 50, 25);
		ofDrawRectRounded(XCntr - 130+5, 520+5, 260, 50, 25);
		// 버튼 본체
		ofSetColor(192);
		ofDrawRectRounded(XCntr - 130 , 440, 260, 50, 25);
		ofDrawRectRounded(XCntr - 130, 520, 260, 50, 25);
		// 버튼 텍스트
		ofSetColor(0, 0, 0);
		myFont16.drawString("press S to Start", XCntr - 80, 470);
		myFont16.drawString("How To Play (H)", XCntr - 80, 550);
		break;
	case 2:
		// 게임설명 화면 : 흰색 배경
		ofSetBackgroundColor(255, 255, 255);
		// 게임 설명 사진 출력
		ofSetColor(255);
		tipTitle.draw(XCntr - 200, 10); // 제목
		tip.draw(XCntr - 400, 100); // 내용
		// 버튼의 그림자
		ofSetColor(0);
		ofDrawRectRounded(XCntr - 130+5, 660+5, 260, 50, 25);
		// 버튼 본체
		ofSetColor(192);
		ofDrawRectRounded(XCntr - 130, 660, 260, 50, 25);
		// 버튼 텍스트
		ofSetColor(0, 0, 0);
		myFont16.drawString("press S to Start", XCntr - 80, 690);
		break;
	case 3:
		// 메뉴화면 : 회색 배경
		ofSetBackgroundColor(224, 224, 224);
		// 메뉴 제목 출력 (Select Stage)
		ofSetColor(255);
		StageTitle.draw(XCntr - 290, 100);
		// 버튼 출력
		char TBuf[100]; // 시간초 출력용 버퍼
		for (int i = 0; i < Stage_Num; i++) // 각 스테이지의 버튼 출력
		{
			// 버튼의 그림자
			ofSetColor(64);
			ofDrawRectRounded(XCntr-512+BArr[i].X+5, BArr[i].Y+5, 140, 180, 20);
			// 버튼 본체
			ofSetColor(255, 255, 255); // 해금시 흰색
			if (Stage_Clear < i) ofSetColor(0, 255, 255); // 미해금시 cyan색
			ofDrawRectRounded(XCntr - 512 + BArr[i].X, BArr[i].Y, 140, 180, 20);
			// 버튼의 이름 (스테이지 이름)
			ofSetColor(0,0,255);
			myFont20.drawString(BArr[i].name, XCntr - 512 + BArr[i].X+15, BArr[i].Y +35);
			// 최대 코인 갯수 출력
			ofSetColor(255, 255, 0); // 얻은 갯수 : 노란색, 미획득 갯수는 모두 회색으로 칠해짐
			if (Star_Clear[i] < 1) ofSetColor(192);
			ofDrawCircle(XCntr - 512 + BArr[i].X+40, BArr[i].Y+70, 10);
			if (Star_Clear[i] < 2) ofSetColor(192);
			ofDrawCircle(XCntr - 512 + BArr[i].X+70, BArr[i].Y+70, 10);
			if (Star_Clear[i] < 3) ofSetColor(192);
			ofDrawCircle(XCntr - 512 + BArr[i].X+100, BArr[i].Y+70, 10);
			// 최단시간 항목 이름 출력 (Time)
			ofSetColor(64);
			myFont24.drawString("Time", XCntr - 512 + BArr[i].X+10, BArr[i].Y+120);
			// 최단시간 출력 (소숫점 2자리까지 출력함)
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
		// 게임오버 화면 : 흰색 배경
		ofSetBackgroundColor(255);
		// 게임오버 글자 그림자
		ofSetColor(64, 0, 0);
		myFont60.drawString("Game Over!", XCntr - 250+2, 300+2);
		// 게임오버 글자 본체
		ofSetColor(255,0,0);
		myFont60.drawString("Game Over!", XCntr - 250, 300);

		// 버튼 그림자
		ofSetColor(128);
		ofDrawRectangle(XCntr - 300+5, 440+5, 260, 80);
		ofDrawRectangle(XCntr + 30+5, 440+5, 260, 80);
		// 재시도 버튼 본체
		ofSetColor(255, 224, 224);
		ofDrawRectangle(XCntr - 300, 440, 260, 80);
		// 메뉴로 돌아가기 버튼 본체
		ofSetColor(255, 128, 128);
		ofDrawRectangle(XCntr + 30, 440, 260, 80);
		// 버튼 스트링 출력
		ofSetColor(128);
		myFont16.drawString("Retry Stage (R)", XCntr - 260, 485);
		myFont16.drawString("Return To Menu (Q)", XCntr+60, 485);
		break;
	case 5:
		// 게임 클리어 화면 : 흰색 배경
		ofSetBackgroundColor(255);
		// 클리어 글자 그림자
		ofSetColor(0, 64, 64);
		myFont60.drawString("Stage Clear!", XCntr - 260+2, 240+2);
		// 클리어 글자 본체
		ofSetColor(0, 255, 255);
		myFont60.drawString("Stage Clear!", XCntr - 260, 240);
		
		// 점수 계산
		if (Time_Clear[prevScreen - 10] == 0 || Time_Clear[prevScreen - 10] > Box.Time) // 최단시간일 경우 최단시간 배열을 갱신함 (Time_Clear가 0이면 첫 클리어이므로 무조건 저장함)
			Time_Clear[prevScreen - 10] = Box.Time;
		if (Stage_Clear <= prevScreen - 10) // 스테이지 클리어 수 갱신
			Stage_Clear = prevScreen + 1 - 10;
		// 점수 계산 공식 : (먹은 동전 갯수 * 25 + 100*남은시간 비율) * 1(힌트 미사용시) or *0.5(힌트 사용시)
		score = (Box.Coin * 25 + 100 * (Limit_Clear[prevScreen - 10] - Box.Time) / Limit_Clear[prevScreen - 10]) / (2 - Box.hint);
		// 점수로 획득 코인 계산 (>=75 :3개, 50~75 : 2개, 25~50 : 1개, <25 : 0개)
		if (score >= 75)
			rtemp = 3;
		else if (score >= 50)
			rtemp = 2;
		else if (score >= 25)
			rtemp = 1;
		else 
			rtemp = 0;

		if (Star_Clear[prevScreen - 10] < rtemp) // 최고 코인 갯수 갱신
			Star_Clear[prevScreen - 10] = rtemp;
		update_file(); // save.txt에 결과를 갱신함

		// 획득 코인 갯수 출력 (획득 : 노랑색, 미획득 : 회색)
		ofSetColor(255, 255, 0);
		if (rtemp < 1) ofSetColor(192);
		ofDrawCircle(XCntr - 160, 360, 50);
		if (rtemp < 2) ofSetColor(192);
		ofDrawCircle(XCntr , 360, 50);
		if (rtemp < 3) ofSetColor(192);
		ofDrawCircle(XCntr + 160, 360, 50);

		// 버튼 출력 
		if (prevScreen == 14) // 마지막 스테이지 (다음 스테이지 버튼 없음)
		{
			// 버튼 그림자
			ofSetColor(128);
			ofDrawRectangle(XCntr - 135+5, 475+5, 260, 80);
			// 버튼 몸체
			ofSetColor(128, 255, 255);
			ofDrawRectangle(XCntr - 135, 475, 260, 80);
			// 버튼 스트링
			ofSetColor(128);
			myFont16.drawString("Return To Menu (Q)", XCntr - 105, 520);
		}
		else // 1~4스테이지 
		{
			// 버튼 그림자
			ofSetColor(128);
			ofDrawRectangle(XCntr - 300+5, 475+5, 260, 80);
			ofDrawRectangle(XCntr + 30+5, 475+5, 260, 80);
			// 다음 스테이지 버튼 몸체
			ofSetColor(192, 255, 255);
			ofDrawRectangle(XCntr - 300, 475, 260, 80);
			// 메뉴로 돌아가기 버튼 몸체
			ofSetColor(128, 255, 255);
			ofDrawRectangle(XCntr + 30, 475, 260, 80);
			// 버튼 스트링
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
		// 게임 플레이 화면
		int Stage = Screen - 10; // 현재 스테이지 수

		double MXCntr = Box.baseX + Box.L * MArr[Stage].Col / 2; // 미로의 중심 X좌표 
		double MYCntr = Box.baseY + Box.L * MArr[Stage].Row / 2; // 미로의 중심 Y좌표
		double MXSize = Box.L * MArr[Stage].Col; // 미로의 가로 크기 
		double MYSize = Box.L * MArr[Stage].Row; // 미로의 세로 크기
		double MScrdx = Box.baseX - Box.defaultX; // 첫 DrawMaze호출시 화면크기와 현재 화면크기의 편차값 (X좌표)
		double MScrdy = Box.baseY - Box.defaultY; // 첫 DrawMaze호출시 화면크기와 현재 화면크기의 편차값 (Y좌표)

		// 흰색배경
		ofSetBackgroundColor(255);
		// 오른쪽의 회색 상태창 출력
		ofSetColor(224);
		ofDrawRectangle(XCntr + 250, 0, ofGetWidth() - (XCntr + 250), ofGetHeight());
		double Side = XCntr + 250; // 상태창 시작 X좌표

		Box.Time = ofGetElapsedTimef() - Box.StartTime; // 경과시간 측정
		Matrix_Clear(Matrix); // 변환용 행렬을 초기화함 (Matrix = I)

		// ↓ Stage 미로 변환 코드 (행렬에 변환식을 곱하는 형태로 중첩됨)
		if (Stage == 0)
		{
			// 1스테이지 : 초당 4도씩 시계방향으로 회전
			Stack_RotateAt(Box.Time * 4, MXCntr, MYCntr); // 중심위치에서 회전
		}
		else if (Stage == 1)
		{
			// 2스테이지 : X방향으로 45초 주기로 전단변형 + 45초 경과시 X축으로 뒤집음 
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
			// 3스테이지 : 10초 주기로 양쪽으로 최대 30도씩 회전함 (등가속운동)
			double prt = 2.5, Angle, MaxAngle = 30, MaxVelocity = 2 * MaxAngle / prt;
			double Cycle = fmodl(Box.Time, 4*prt);

			// MaxAngle = 0.5*AngluarAcceleration*(주기/4)^2 형태로 진동함 (↗↙↖↘순)
			if (Cycle <= prt) // ↗ 방향 이동
				Angle = MaxVelocity * Cycle * (1 - Cycle / 2 / prt);
			else if (Cycle <= prt * 2) // ↙ 방향 이동
			{
				Cycle = 2 * prt - Cycle;
				Angle = MaxVelocity * Cycle * (1 - Cycle / 2 / prt);
			}
			else if (Cycle <= prt * 3) // ↖ 방향 이동
			{
				Cycle = Cycle - 2 * prt;
				Angle = -MaxVelocity * Cycle * (1 - Cycle / 2 / prt);
			}
			else // ↘ 방향 이동
			{
				Cycle = 4 * prt - Cycle;
				Angle = -MaxVelocity * Cycle * (1 - Cycle / 2 / prt);
			}

			Stack_RotateAt(Angle, MXCntr, MYCntr); // 회전변환
			Stack_Move(-Box.L * 1, Box.L * 1.5); // 위치 가운데로 맞춤
		}
		else if (Stage == 3)
		{
			// 4스테이지 : 4.5*4초를 주기로 4.5초마다 90도씩 시계 반대방향으로 회전함
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
			Stack_RotateAt(Angle, MXCntr, MYCntr); // 회전변환
		}
		else if (Stage == 4)
		{
			// 5스테이지 : 초당 16도씩 시계방향으로 회전 -> 오른쪽 변의 크기를 0.5배로 만드는 투영 변환 (RUY ->RUY*0.5, RDY->RDY*0.5) -> 크기 및 위치 변경
			double prt = 45 / 2;
			Stack_RotateAt(360 / prt * Box.Time, MXCntr, MYCntr); // 회전 변환
			Stack_Move((512 - XCntr)*1.4, 0); // 화면 크기를 고려해 위치이동
			Stack_Project(Stage,mapX(0),mapY(0), mapX(0), mapY(MArr[Stage].Row), mapX(MArr[Stage].Col), mapY(MArr[Stage].Row / 4), mapX(MArr[Stage].Col), mapY(MArr[Stage].Row * 3 / 4)); // 투영 변환
			Stack_Scale(1.4, 1.4); // 크기 조절
			Stack_Move(-MXCntr / 2 - (512 - XCntr) * 1, -MYCntr / 2); // 위치 변경
		}

		Stack_Move((512 - XCntr) / 2,0); // 필드 영역 가운데 정렬 
		// ↑ 스테이별 커스텀 코드 영역

		Matrix_Inverse(Matrix, Inverse); // 중첩된 변환행렬의 역행렬 계산

		// Draw Maze (힌트 사용시 DFS 사용으로 힌트도 출력함, 첫 호출시 현재 화면 크기를 기준으로 미로의 Orig, WOrig, WType을 계산함)
		DrawMaze(Stage);

		// 벽의 변환후 충돌판정을 계산함
		for (int k = 0; k < MArr[Stage].Orig.size(); k++) // Orig (변환전 충돌판정) 순회
		{
			Block BlockT = MArr[Stage].Orig[k];
			for (int l = 0; l < 4; l++) // 현재 화면 크기로 보정함 (평행이동)
			{
				BlockT.P[l].X += MScrdx; 
				BlockT.P[l].Y += MScrdy;
			}
			MArr[Stage].Trans[k] = Matrix_TransBlock(Matrix, BlockT); // 위에서 구한 변환행렬로 Trans (변환후 충돌판정) 계산
		}
		int l = 0;
		for (int k = 0; k < MArr[Stage].WOrig.size(); k++) // WOrig (변환전 벽-물 충돌판정) 순회
			if (MArr[Stage].Wtype[k] != 3) // 임시용 타입 3번은 제외
			{
				MArr[Stage].WTest[l] = MArr[Stage].Wtype[k]; // WTest에 현재 Block의 타입 그대로 전달 (Wtype)

				Line LinT = MArr[Stage].WOrig[k];
				LinT.x1 += MScrdx; // 현재 화면 크기로 보정함 (평행이동)
				LinT.y1 += MScrdy;
				LinT.x2 += MScrdx;
				LinT.y2 += MScrdy;
				MArr[Stage].Wall[l++] = Matrix_TransLine(Matrix, LinT); // 위에서 구한 변환행렬로 Wall (변환후 벽-물 충돌판정) 계산
			}
		// Draw Water
		for (int k = 0; k < WaterNum[Stage]; k++) // 물줄기 배열 순회
		{
			double Timer = ofGetElapsedTimef() - MArr[Stage].Hole[k].StartTime + MArr[Stage].Hole[k].TotalTime; 
			double N = (Timer / MArr[Stage].Hole[k].TotalTime);
			double R = (N - (int)N) * MArr[Stage].Hole[k].TotalTime; // StartTime부터 시작하는 주기가 TotalTime인 타이머
			if (R <= MArr[Stage].Hole[k].EnableTime) // 물줄기가 해당 타이머에서 EnableTime 구간을 지나는 경우 실행
				MArr[Stage].Hole[k].run = 1; // 물줄기 실행
			else
				MArr[Stage].Hole[k].run = 0; // 물줄기 중단
		}

		DrawWater(Stage); // 물줄기 및 용암 경로 계산후 그림

		// 플레이어가 물줄기 및 용암에 닿았는지를 체크
		for (int l = 0; l < MArr[Stage].Hole.size(); l++) // 물줄기 배열 순회
		{
			vector<Block>&BArr = MArr[Stage].Hole[l].wblock; // wblock : 물-플레이어 충돌판정 배열
			for (int k = 0; k < MArr[Stage].Hole[l].size - 1; k++) // 현재 물줄기 - 플레이어 충돌판정을 순회
			{
				if (MArr[Stage].Hole[l].run == 0) continue; // 물줄기가 실행중이 아니면 pass
				if (CheckCollide(BArr[k])) // 현재 block과 플레이어가 충돌했는지 check
				{
					if (MArr[Stage].Hole[l].type == 0) // 닿은 박스가 물줄기 -> 둔화 실행
					{
						Box.SlowStart = ofGetElapsedTimef();
						Box.Slowflag = 1;
					}
					else // 닿은 박스가 불줄기 -> 게임오버
					{
						ofClear(255);
						Screen = 4;
					}
				}
			}
		}

		// Draw Character
		Box.TLoc = Matrix_Trans(Matrix, Box.Loc); // 변환후 플레이어 위치 계산

		// 변환후 위치를 기준으로 상하좌우*Box.speed 만큼 이동 -> 역행렬로 변환전 이동거리를 연산함
		Box.Move.P[0] = { Box.TLoc.X,Box.TLoc.Y - Box.speed }; // U
		Box.Move.P[1] = { Box.TLoc.X,Box.TLoc.Y + Box.speed }; // D
		Box.Move.P[2] = { Box.TLoc.X - Box.speed,Box.TLoc.Y }; // L
		Box.Move.P[3] = { Box.TLoc.X + Box.speed,Box.TLoc.Y }; // R
		Box.Move = Matrix_TransBlock(Inverse, Box.Move); // 일반 이동 거리 역산
		Box.SlowMove.P[0] = { Box.TLoc.X,Box.TLoc.Y - Box.speed/2 }; // U
		Box.SlowMove.P[1] = { Box.TLoc.X,Box.TLoc.Y + Box.speed/2 }; // D
		Box.SlowMove.P[2] = { Box.TLoc.X - Box.speed/2,Box.TLoc.Y }; // L
		Box.SlowMove.P[3] = { Box.TLoc.X + Box.speed/2,Box.TLoc.Y }; // R
		Box.SlowMove = Matrix_TransBlock(Inverse, Box.SlowMove); // 둔화 이동 거리 역산
		for (int k = 0; k < 4; k++) // 이동후 위치 - 이동전 위치로 이동거리를 결정함
		{
			Box.Move.P[k].X -= Box.Loc.X;
			Box.Move.P[k].Y -= Box.Loc.Y;
			Box.SlowMove.P[k].X -= Box.Loc.X;
			Box.SlowMove.P[k].Y -= Box.Loc.Y;
		}

		// 플레이어 캐릭터 : 분홍색 박스
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
			if (Box.GetCoin[i] == 0) // 동전을 먹지 않은경우 그림
				ofDrawCircle(Ptemp.X, Ptemp.Y, CoinR);
			if (Box.GetCoin[i] == 0 && CheckCircle(Ptemp.X, Ptemp.Y, CoinR)) // 동전에 플레이어가 닿은경우 해당 동전을 먹은것으로 처리
			{
				Box.GetCoin[i] = 1;
				Box.Coin++;
			}
		}

		// Draw Text
		// 스테이지 명
		ofSetColor(0, 0, 255);
		char sname[] = { "Stage 10" };
		if (Stage < 9)
		{
			sname[7] = '\0';
			sname[6] = '1' + Stage;
		}
		myFont20.drawString(sname, Side + 20, 80);

		// 시간 제한
		ofSetColor(128);
		myFont20.drawString("Time Limit", Side + 20, 140);
		myFont20.drawString(to_string(Limit_Clear[Stage] - Box.Time), Side + 20, 175);

		// 힌트 남은갯수
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
			if (Box.hintTimer > 0) // 힌트 사용시 남은시간을 주황색 게이지 바로 표시함
			{
				ofSetColor(256, 128, 0);
				ofDrawRectangle(Side + 20, 240, (210 - Box.hintTimer * 21), 30);
			}
		}

		// 먹은 코인 갯수
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

		// 둔화 상태일경우 초록색 게이지 바로 남은시간 표시 
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
				Box.Slowflag = 0; // 5초가 지나면 둔화 해제
		}

		// Clear
		if (CheckCircle(TDest.X, TDest.Y, DestR)) // 도착점에 닿으면 승리
		{
			ofClear(255);
			Screen = 5;
		}
		// Fail
		if (Limit_Clear[Stage] - Box.Time < 0) // 시간제한 넘으면 게임오버
		{
			ofClear(255);
			Screen = 4;
		}
		break;
	}
} 

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (Screen) // 각 화면에서의 키보드 인식 코드 실행
	{
	case 1: // 시작화면
		if (key == 's' || key == 'S') { // 메뉴화면으로 이동
			ofClear(224);
			Screen = 3;
		}
		else if (key == 'h' || key == 'H') // 게임설명으로 이동
		{
			ofClear(255, 255, 255);
			Screen = 2;
		}
		break;
	case 2: // 게임설명 화면
		if (key == 's' || key == 'S') { // 메뉴화면으로 이동
			ofClear(224);
			Screen = 3;
		}
		else if (key == 'q' || key == 'Q') // 시작화면으로 이동
		{
			ofClear(255, 255, 255);
			Screen = 1;
		}
		break;
	case 3: // 스테이지 선택창 (메뉴화면)
		if (key == 'q' || key == 'Q') // 시작화면으로 이동
		{
			ofClear(255, 255, 255);
			Screen = 1;
		}
		// 1~5스테이지 각각 1~5번키로 시작하는 함수 (해당 스테이지가 해금된 경우만 가능, 시작시 해당 스테이지를 초기화함)
		else if (key == '1' && Stage_Clear >= 0) // 1스테이지 시작
		{
			ofClear(255);
			Screen = 10 + 0;
			prevScreen = 10 + 0;
			StageReset(0);
		}
		else if (key == '2' && Stage_Clear >= 1) // 2스테이지 시작
		{
			ofClear(255);
			Screen = 10 + 1;
			prevScreen = 10 + 1;
			StageReset(1);
		}
		else if (key == '3' && Stage_Clear >= 2) // 3스테이지 시작
		{
			ofClear(255);
			Screen = 10 + 2;
			prevScreen = 10 + 2;
			StageReset(2);
		}
		else if (key == '4' && Stage_Clear >= 3) // 4스테이지 시작
		{
			ofClear(255);
			Screen = 10 + 3;
			prevScreen = 10 + 3;
			StageReset(3);
		}
		else if (key == '5' && Stage_Clear >= 4) // 5스테이지 시작
		{
			ofClear(255);
			Screen = 10 + 4;
			prevScreen = 10 + 4;
			StageReset(4);
		}

		break;
	case 4: // 게임오버 화면
		if (key == 'q' || key == 'Q') // 메뉴화면으로 이동
		{
			ofClear(224);
			Screen = 3;
		}
		else if (key == 'r' || key == 'R') // 재시작
		{
			ofClear(255, 255, 255);
			// 이전 화면에 저장된 스테이지를 다시 플레이함
			Screen = prevScreen;
			StageReset(prevScreen-10);
		}
		break;
	case 5: // 클리어 화면
		if (key == 'q' || key == 'Q') // 메뉴화면으로 이동
		{
			ofClear(224);
			Screen = 3;
		}
		else if (prevScreen != 14 && (key == 's' || key == 'S')) // 다음 스테이지로
		{
			ofClear(255, 255, 255);
			// 다음 스테이지를 초기화하고 플레이험
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
		// 1~5스테이지의 키인식 처리 함수
		double dx, dy, Tdx, Tdy; // 각각 변환전 x, y좌표 변화량, 변환후 x, y좌표 변화량
		if (key == 'q' || key == 'Q') // 메뉴화면으로 이동
		{
			ofClear(224);
			Screen = 3;
		}
		if (Box.hint == 1 &&(key == 'h' || key == 'H')) // 힌트 사용
		{
			Box.hint = 0;
			Box.hintTimer = 10;
			Box.hintStartTime = ofGetElapsedTimef(); // 힌트 시작시간을 현재시간으로 설정
		}
		if (key == OF_KEY_UP) // 화살표 ↑키
		{
			if (Box.Slowflag == 0) // 일반
			{
				Tdy = -Box.speed;
				dx = Box.Move.P[0].X;
				dy = Box.Move.P[0].Y;
			}
			else // 둔화
			{
				Tdy = -Box.slow_speed;
				dx = Box.SlowMove.P[0].X;
				dy = Box.SlowMove.P[0].Y;
			}

			// 윗방향 이동 (CheckMaze로 충돌체크 후, 가능하면 변환전 좌표를 움직여 실제로 플레이어 캐릭을 이동시킴)
			Box.TLoc.Y += Tdy; 
			if(CheckMaze(Screen-10)) // 충돌체크는 변환후 좌표를 기준으로함
				Box.TLoc.Y -= Tdy;
			else {
				Box.Loc.X += dx;
				Box.Loc.Y += dy;
			}
		}
		if (key == OF_KEY_DOWN) // 화살표 ↓키
		{
			if (Box.Slowflag == 0)  // 일반
			{
				Tdy = Box.speed;
				dx = Box.Move.P[1].X;
				dy = Box.Move.P[1].Y;
			}
			else // 둔화
			{
				Tdy = Box.slow_speed;
				dx = Box.SlowMove.P[1].X;
				dy = Box.SlowMove.P[1].Y;
			}

			// 아래방향 이동 (CheckMaze로 충돌체크 후, 가능하면 변환전 좌표를 움직여 실제로 플레이어 캐릭을 이동시킴)
			Box.TLoc.Y += Tdy;
			if (CheckMaze(Screen - 10)) // 충돌체크는 변환후 좌표를 기준으로함
				Box.TLoc.Y -= Tdy;
			else {
				Box.Loc.X += dx;
				Box.Loc.Y += dy;
			}
		}
		if (key == OF_KEY_LEFT) // 화살표 ←키
		{
			if (Box.Slowflag == 0)  // 일반
			{
				Tdx = -Box.speed;
				dx = Box.Move.P[2].X;
				dy = Box.Move.P[2].Y;
			}
			else // 둔화
			{
				Tdx = -Box.slow_speed;
				dx = Box.SlowMove.P[2].X;
				dy = Box.SlowMove.P[2].Y;
			}

			// 왼쪽방향 이동 (CheckMaze로 충돌체크 후, 가능하면 변환전 좌표를 움직여 실제로 플레이어 캐릭을 이동시킴)
			Box.TLoc.X += Tdx;
			if (CheckMaze(Screen - 10)) // 충돌체크는 변환후 좌표를 기준으로함
				Box.TLoc.X -= Tdx;
			else {
				Box.Loc.X += dx;
				Box.Loc.Y += dy;
			}
		}
		if (key == OF_KEY_RIGHT) // 화살표 →키
		{
			if (Box.Slowflag == 0)  // 일반
			{
				Tdx = Box.speed;
				dx = Box.Move.P[3].X;
				dy = Box.Move.P[3].Y;
			}
			else // 둔화
			{
				Tdx = Box.slow_speed;
				dx = Box.SlowMove.P[3].X;
				dy = Box.SlowMove.P[3].Y;
			}

			// 오른쪽방향 이동 (CheckMaze로 충돌체크 후, 가능하면 변환전 좌표를 움직여 실제로 플레이어 캐릭을 이동시킴)
			Box.TLoc.X += Tdx;
			if (CheckMaze(Screen - 10)) // 충돌체크는 변환후 좌표를 기준으로함
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
	int XCntr = ofGetWidth() / 2; // 화면의 중간 위치 (X좌표)
	switch (Screen) // 각 화면에서의 마우스 클릭 인식 코드 실행
	{
	case 1: // 시작화면
		if (x >= XCntr-130 && x <= XCntr+130 && y >= 440 && y <= 490) { // 메뉴화면으로 이동
			ofClear(224);
			Screen = 3;
		}
		else if (x >= XCntr - 130 && x <= XCntr + 130 && y >= 520 && y <= 570) // 게임설명으로 이동
		{
			ofClear(255, 255, 255);
			Screen = 2;
		}
		break;
	case 2: // 게임설명
		if (x >= XCntr - 130 && x <= XCntr + 130 && y >= 660 && y <= 710) { // 메뉴화면으로 이동
			ofClear(224);
			Screen = 3;
		}
		break;
	case 3: // 스테이지 선택창 (메뉴화면)
		for (int i = 0; i < Stage_Num; i++)
		{
			if (Stage_Clear >= i && x >= XCntr - 512 + BArr[i].X && x <= XCntr - 512 + BArr[i].X+140 && y >= BArr[i].Y && y <= BArr[i].Y + 180) { // 해당 스테이지가 해금되어있다면, 각 스테이지로 이동함 
				ofClear(255);
				Screen = 10+i;
				prevScreen = 10 + i; 
				StageReset(i); // 스테이지 초기화
			}
		}
		break;
	case 4: // 게임오버 화면
		if (x >= XCntr - 300 && x <= XCntr - 40 && y >= 440 && y <= 520) { // 재시도
			ofClear(255);
			// 이전 화면으로 돌아가서 해당 스테이지를 다시 플레이함
			Screen = prevScreen;
			StageReset(prevScreen - 10);
		}
		else if (x >= XCntr - 30 && x <= XCntr + 230 && y >= 440 && y <= 520) { // 메뉴화면으로 이동
			ofClear(224);
			Screen = 3;
		}
		break;
	case 5: // 클리어 화면
		if (prevScreen == 14) // 마지막 스테이지 클리어시
		{
			if (x >= XCntr - 135 && x <= XCntr + 125 && y >= 475 && y <= 555) { // 메뉴화면으로 이동
				ofClear(224);
				Screen = 3;
			}
		}
		else // 1~4스테이지 클리어시
		{
			if (x >= XCntr - 300 && x <= XCntr - 40 && y >= 475 && y <= 555) { // 다음 스테이지
				ofClear(255);
				// 다음 스테이지를 플레이함
				Screen = prevScreen + 1;
				StageReset(prevScreen + 1 - 10);
				prevScreen += 1;
			}
			else if (x >= XCntr - 30 && x <= XCntr + 230 && y >= 475 && y <= 555) { // 메뉴화면으로 이동
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

void SetCoinLoc(int Num, int Limit) // 코인의 위치를 재설정함
{
	for (int i = 0; i < 3; i++) // 총 코인 갯수 : 3개
	{
		while (1)
		{
			Box.GetCoin[i] = 0; // 먹었는지 여부를 초기화
			Box.CoinDest[i].Y = (int)ofRandom(0, MArr[Num].Row); // X,Y좌표 미로 내부의 랜덤위치로 설정
			Box.CoinDest[i].X = (int)ofRandom(0, MArr[Num].Col);
			if (Box.CoinDest[i].Y + Box.CoinDest[i].X > Limit || Box.CoinDest[i].Y + Box.CoinDest[i].X < MArr[Num].Col + MArr[Num].Row - 2 - Limit) // 코인의 위치가 정상 범위에 있는경우 탈출 (Limit 고려_
				break;
		}
	}
}

void SetStageSetting(int Num, double Zoom)  // 스테이지를 설정을 초기화함
{
	if (MArr[Num].Orig.size() == 0) // 처음 해당 스테이지를 실행한 경우
	{
		ScrX[Num] = ofGetWidth(); // DrawMaze 첫 호출시 사용할 화면 크기값을 각각 저장함
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

	for (int i = 0; i < 3; i++) // 동전의 좌표를 index에서 화면좌표로 변환함
	{
		Box.CoinDest[i].X = Box.baseX + Box.L * (Box.CoinDest[i].X + 0.5);
		Box.CoinDest[i].Y = Box.baseY + Box.L * (Box.CoinDest[i].Y + 0.5);
	}
}

void StageReset(int Num) // 스테이지를 초기화함
{
	// 모든 스테이지 공통 사항
	Box.Time = 0.;
	Box.Coin = 0.;
	Box.SlowTimer = 0.;
	Box.size = 15;
	Box.hint = 1;
	Box.StartTime = ofGetElapsedTimef(); // 시작시간 저장
	Box.Slowflag = 0;
	Matrix_Clear(Matrix); // 변환용 행렬을 초기화함 (Matrix = I)
	Box.hintTimer = 0;
	for (int i = 0; i < 4; i++)
	{
		Box.Move.P[i] = { 0,0 };
		Box.SlowMove.P[i] = { 0,0 };
	}
	SetCoinLoc(Num, 3); // 동전위치 초기화
	
	int loc = 0; // 임시변수
	// Stage별 초기화 사항
	switch (Num)
	{
	case 0: // Stage 1
		SetStageSetting(Num, 2); // 스테이지 설정 초기화
		
		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 1스테이지 : 2개의 물줄기를 미로 내부의 임의의 점에 생성함, 6초 주기로 4.5초씩 동시에 발사
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
		SetStageSetting(Num, 1.8); // 스테이지 설정 초기화

		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 2스테이지 : 2개의 물줄기와 용암을 각각 상하좌우 맨 끝줄의 임의의 점에 생성함, 8초 주기로 2초씩 1개를 번갈아가며 발사
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

		MArr[Num].Hole[0].Dot = { mapX(0.5),mapY(0.5 + (int)ofRandom(1, MArr[Num].Row - 1))}; // 좌
		MArr[Num].Hole[1].Dot = { mapX(0.5 + (int)ofRandom(1, MArr[Num].Col - 1)),mapY(0.5)}; // 상
		MArr[Num].Hole[2].Dot = { mapX(0.5 + MArr[Num].Col - 1),mapY(0.5 + (int)ofRandom(1, MArr[Num].Row - 1)) }; // 우
		MArr[Num].Hole[3].Dot = { mapX(0.5 + (int)ofRandom(1, MArr[Num].Col - 1)),mapY(0.5 + MArr[Num].Row - 1) }; // 하
		break;
	case 2: // Stage 3
		SetStageSetting(Num, 2); // 스테이지 설정 초기화

		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 3스테이지 : 10개의 물줄기를 맨 윗줄에 생성, 5초 주기로 0.5초씩 왼쪽부터 오른쪽까지 차례대로 1개씩 발사
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
		SetStageSetting(Num, 1.8); // 스테이지 설정 초기화

		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 4스테이지 : 12개의 용암을 상하좌우 맨 끝줄에 3개씩 2칸의 간격을 두고 생성 (간격 시작위치 0~2중 랜덤), 4.5*4초 주기로 4.25초씩 각 변마다 번갈아가며 발사
			MArr[Num].Hole[i].StartTime = Box.StartTime + 4.5 * (i / 3);
			MArr[Num].Hole[i].EnableTime = 4.25;
			MArr[Num].Hole[i].TotalTime = 4.5*4;
			MArr[Num].Hole[i].t = 7.5 * Box.zoom;
			MArr[Num].Hole[i].run = 0;
			MArr[Num].Hole[i].type = 1;
			if (i % 3 == 0)
				loc = (int)ofRandom(0, 3); // 간격 시작위치 랜덤화 (0~2)
			int q = i / 3, r = i % 3;
			if (q == 0)
				MArr[Num].Hole[i].Dot = { mapX(1.5 + loc + r * 3), mapY(0.2) }; // 맨 윗줄
			else if (q == 1)
				MArr[Num].Hole[i].Dot = { mapX(0.8 + MArr[Num].Col - 1), mapY(1.5 + loc + r * 3) }; // 맨 오른쪽 줄
			else if (q == 2)
				MArr[Num].Hole[i].Dot = { mapX(1.5 + loc + r * 3), mapY(0.8 + MArr[Num].Row - 1) }; // 맨 아랫줄 
			else if (q == 3)
				MArr[Num].Hole[i].Dot = { mapX(0.2), mapY(1.5 + loc + r * 3) }; // 맨 왼쪽 줄
		}
		break;
	case 4: // Stage 5
		SetStageSetting(Num, 2.2); // 스테이지 설정 초기화

		for (int i = 0; i < WaterNum[Num]; i++)
		{
			// 5스테이지 : 4개의 물줄기와 4개의 용암을 필드를 각 사분면마다 1개씩 생성, 16초 주기로 4.5씩 각 사분면의 물줄기와 용암을 발사
			MArr[Num].Hole[i].StartTime = Box.StartTime + 4 * (i / 2);
			MArr[Num].Hole[i].EnableTime = 4.5;
			MArr[Num].Hole[i].TotalTime = 16;
			MArr[Num].Hole[i].t = 7.5 * Box.zoom;
			MArr[Num].Hole[i].run = 0;
			if (i % 2 == 0)
				MArr[Num].Hole[i].type = 0;
			else
				MArr[Num].Hole[i].type = 1;

			int rx = (int)ofRandom(0, 4), ry = (int)ofRandom(0, 4); // 사분면 내의 랜덤 위치를 설정
			int q = i / 2;
			if (q == 0)
			{
				if (rx == 0 && ry == 0) // 2사분면의 시작점 위치(0,0)는 (1,1)로 옮겨 시작부터 패배하는 상황 제외함
					rx = 1, ry = 1;
				MArr[Num].Hole[i].Dot = { mapX(0.5 + rx), mapY(0.5 + ry) }; // 2사분면
			}
			else if (q == 1)
				MArr[Num].Hole[i].Dot = { mapX(4.5 + rx), mapY(0.5 + ry) }; // 1사분면
			else if (q == 2)
				MArr[Num].Hole[i].Dot = { mapX(4.5 + rx), mapY(4.5 + ry) }; // 3사분면
			else if (q == 3)
				MArr[Num].Hole[i].Dot = { mapX(0.5 + rx), mapY(4.5 + ry) }; // 4사분면

			if (i % 2 == 1 && (MArr[Num].Hole[i].Dot.X == MArr[Num].Hole[i-1].Dot.X) && (MArr[Num].Hole[i].Dot.Y == MArr[Num].Hole[i - 1].Dot.Y)) // 사분면의 점이 겹칠경우 
			{
				if (ry <= 2) // (y+1)%4 연산을 수행함
					MArr[Num].Hole[i].Dot.Y += mapY(1) - mapY(0);
				else 
					MArr[Num].Hole[i].Dot.Y -= mapY(3) - mapY(0);
			}
		}
		break;
	}
}

void update_file() // 세이브 데이터를 파일로 출력하는 함수
{
	// bin/data 폴더의 save.txt파일에 클리어한 스테이지 수, 스테이지별 최대 코인갯수, 최단시간이 기록됨
	char fname[] = "save.txt";
	fopen_s(&fptr, ofToDataPath(fname).c_str(), "w");
	fprintf(fptr, "%d\n", Stage_Clear); // total clear Stage
	for (int i = 0; i < Stage_Num; i++)
		fprintf(fptr, "%d %lf\n", Star_Clear[i], Time_Clear[i]); // Star, Time
	fclose(fptr);
}

void Ensnare() // 둔화 상태를 설정하는 함수
{
	Box.SlowStart = ofGetElapsedTimef(); // 현재시간부터 둔화시작
	Box.Slowflag = 1;
}
