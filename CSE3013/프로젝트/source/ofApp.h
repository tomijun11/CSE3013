#pragma once
#include "ofMain.h"

class ofApp : public ofBaseApp {

public:

	void setup(); // 초기설정 : 미로, 폰트, 이미지파일들을 로드함
	void update(); // 미사용
	void draw(); // 게임화면을 그리는 함수

	void keyPressed(int key); // 키인식 관련 함수
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button); // 마우스 클릭인식 관련 함수
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	float windowWidth, windowHeight; // 윈도우창의 폭과 높이
	ofTrueTypeFont myFont; // 사용할 폰트
	ofImage Title, StageTitle, tipTitle, tip; // 사용할 이미지들
};

typedef struct _point { // 점의 좌표를 저장하는 구조체
	double X; // X좌표
	double Y; // Y좌표
} Point;

typedef struct _room { // 미로의 각 방을 나타내는 구조체 (1 : 벽 존재, 0 : 벽 없음)
	int visited; // 중복방지 
	int r; // 우측 벽
	int d; // 아래 벽
}Room;

typedef struct _node { // DFS 호출시 사용하는 스택
	int row; // Y좌표
	int col; // X좌표
	struct _node* next; // 연결리스트용 포인터
}Node;

typedef struct _block { // 점 4개로 구성되는 직사각형의 자표를 저장하는 구조체
	Point P[4]; // 꼭짓점의 좌표
}Block;

typedef struct _line { // 선분의 좌표를 저장하는 구조체
	float x1; // 시작점의 X좌표
	float y1; // 시작점의 Y좌표
	float x2; // 끝점의 X좌표
	float y2; // 끝점의 Y좌표
}Line;

typedef struct _water { // 물줄기의 정보를 저장하는 구조체
	double TotalTime; // 주기
	double EnableTime; // 한 주기동안 물줄기가 켜지는 시간
	double StartTime; // 물줄기가 처음으로 켜지는데 걸리는 시간 (대기시간)
	double t; // 물줄기의 두께
	int size; // 물줄기 경로의 갯수
	int run; // 물줄기의 작동여부 (0:꺼짐, 1:켜짐)
	int type; // 물줄기의 종류 (0 : 물, 1: 용암)
	Point Dot; // 물줄기 시작점의 좌표
	
	vector<Point> Path; // 물줄기의 경로를 저장한 배열
	vector<Block> wblock; // 물줄기의 접촉 판정을 저장한 배열 (한 직사각형당 Block 1개를 저장함)
}Water;

typedef struct _mdata { // 미로의 정보를 저장하는 구조체
	int Row; // 미로의 세로크기
	int Col; // 미로의 가로크기
	Room** Maze; // 미로의 정보를 저장하는 배열
	vector<Block> Orig; // 변환전 미로의 벽의 위치 (플레이어-벽의 충돌판정에 사용함)
	vector<Block> Trans; // 변환후 미로의 벽의 위치 (플레이어-벽의 충돌판정에 사용함)
	vector<Water> Hole; // 미로 내의 물줄기를 저장하는 배열
	vector<Line> WOrig; // 변환전 미로의 벽의 위치 (물줄기-벽의 충돌판정에 사용함)
	vector<Line> Wall; // 변환후 미로의 벽의 위치 (물줄기-벽의 충돌판정에 사용함)
	vector<int> Wtype; // 미로 벽의 종류 (WTest와 동일하나, DrawMaze최초 호출시 계산되어 WTest에 type값이 3인 항목을 제외한 모든 항목을 WTest에 저장함)
	vector<int> WTest; // 미로 벽의 종류 (물줄기의 경로계산에 사용, 0:세로선 1:가로선 2:두께(가로및세로) 3:임시용(실제로 사용X) 4:가로선(내부) 5:세로선(내부)
}MData;

typedef struct _button { // 스테이지 선택창의 스테이지 버튼의 정보를 저장하는 구조체
	double X; // X좌표
	double Y; // Y좌표
	char name[10]; // 스테이지 이름
}Button;

typedef struct _player { // 플레이어의 정보를 저장하는 구조체
	Point Loc; // 변환전 플레이어 위치 좌표
	Point TLoc; // 변환후 플레이어 위치 좌표
	double SlowTimer; // 남은 둔화시간
	double SlowStart; // 둔화가 시작된 시간
	int Slowflag; // 둔화 상태 (0:정상, 1:둔화 걸림)
	double size; // 플레이어의 크기
	double Time; // 남은시간
	int Coin; // 먹은 동전의 갯수
	int hint; // 남은 힌트 갯수 (1: 사용안함, 0:사용함)
	double baseX; // 미로를 그리기 시작할 X좌표
	double baseY; // 미로를 그리기 시작할 Y좌표
	double defaultX; // 첫 DrawMaze실행시 계산된 baseX (화면크기 보정용)
	double defaultY; // 첫 DrawMaze실행시 계산된 baseY (화면크기 보정용)
	double L; // 미로 한 칸의 길이
	double zoom; // 미로를 확대할 배율
	double speed; // 플레이어 이동속도
	double slow_speed; // 플레이어 둔화시 이동속도
	Point Dest; // 도착점의 위치
	double hintTimer; // 남은 힌트출력 시간
	double StartTime; // 스테이지 시작 시간
	double hintStartTime; // 힌트출력 시작시간
	Point CoinDest[3]; // 동전의 위치 배열
	int GetCoin[3]; // 각 동전의 먹었는지의 여부 (0:안먹음, 1:먹음)
	Block Move; // 이동시 움직일 변위 배열(UDLR순서)
	Block SlowMove; // 둔화중 이동시 움직일 변위 배열(UDLR순서)
}Player;

void SetCoinLoc(int Num, int Limit); // 코인의 위치를 재설정함
void SetStageSetting(int Num, double Zoom); // 스테이지를 설정을 초기화함
void StageReset(int Num); // 스테이지를 초기화함
void update_file(); // 세이브 데이터를 파일로 출력하는 함수
void Ensnare(); // 둔화 상태를 설정하는 함수