#pragma once
#include "ofMain.h"

class ofApp : public ofBaseApp {

public:

	void setup(); // �ʱ⼳�� : �̷�, ��Ʈ, �̹������ϵ��� �ε���
	void update(); // �̻��
	void draw(); // ����ȭ���� �׸��� �Լ�

	void keyPressed(int key); // Ű�ν� ���� �Լ�
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button); // ���콺 Ŭ���ν� ���� �Լ�
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	float windowWidth, windowHeight; // ������â�� ���� ����
	ofTrueTypeFont myFont; // ����� ��Ʈ
	ofImage Title, StageTitle, tipTitle, tip; // ����� �̹�����
};

typedef struct _point { // ���� ��ǥ�� �����ϴ� ����ü
	double X; // X��ǥ
	double Y; // Y��ǥ
} Point;

typedef struct _room { // �̷��� �� ���� ��Ÿ���� ����ü (1 : �� ����, 0 : �� ����)
	int visited; // �ߺ����� 
	int r; // ���� ��
	int d; // �Ʒ� ��
}Room;

typedef struct _node { // DFS ȣ��� ����ϴ� ����
	int row; // Y��ǥ
	int col; // X��ǥ
	struct _node* next; // ���Ḯ��Ʈ�� ������
}Node;

typedef struct _block { // �� 4���� �����Ǵ� ���簢���� ��ǥ�� �����ϴ� ����ü
	Point P[4]; // �������� ��ǥ
}Block;

typedef struct _line { // ������ ��ǥ�� �����ϴ� ����ü
	float x1; // �������� X��ǥ
	float y1; // �������� Y��ǥ
	float x2; // ������ X��ǥ
	float y2; // ������ Y��ǥ
}Line;

typedef struct _water { // ���ٱ��� ������ �����ϴ� ����ü
	double TotalTime; // �ֱ�
	double EnableTime; // �� �⵿ֱ�� ���ٱⰡ ������ �ð�
	double StartTime; // ���ٱⰡ ó������ �����µ� �ɸ��� �ð� (���ð�)
	double t; // ���ٱ��� �β�
	int size; // ���ٱ� ����� ����
	int run; // ���ٱ��� �۵����� (0:����, 1:����)
	int type; // ���ٱ��� ���� (0 : ��, 1: ���)
	Point Dot; // ���ٱ� �������� ��ǥ
	
	vector<Point> Path; // ���ٱ��� ��θ� ������ �迭
	vector<Block> wblock; // ���ٱ��� ���� ������ ������ �迭 (�� ���簢���� Block 1���� ������)
}Water;

typedef struct _mdata { // �̷��� ������ �����ϴ� ����ü
	int Row; // �̷��� ����ũ��
	int Col; // �̷��� ����ũ��
	Room** Maze; // �̷��� ������ �����ϴ� �迭
	vector<Block> Orig; // ��ȯ�� �̷��� ���� ��ġ (�÷��̾�-���� �浹������ �����)
	vector<Block> Trans; // ��ȯ�� �̷��� ���� ��ġ (�÷��̾�-���� �浹������ �����)
	vector<Water> Hole; // �̷� ���� ���ٱ⸦ �����ϴ� �迭
	vector<Line> WOrig; // ��ȯ�� �̷��� ���� ��ġ (���ٱ�-���� �浹������ �����)
	vector<Line> Wall; // ��ȯ�� �̷��� ���� ��ġ (���ٱ�-���� �浹������ �����)
	vector<int> Wtype; // �̷� ���� ���� (WTest�� �����ϳ�, DrawMaze���� ȣ��� ���Ǿ� WTest�� type���� 3�� �׸��� ������ ��� �׸��� WTest�� ������)
	vector<int> WTest; // �̷� ���� ���� (���ٱ��� ��ΰ�꿡 ���, 0:���μ� 1:���μ� 2:�β�(���ι׼���) 3:�ӽÿ�(������ ���X) 4:���μ�(����) 5:���μ�(����)
}MData;

typedef struct _button { // �������� ����â�� �������� ��ư�� ������ �����ϴ� ����ü
	double X; // X��ǥ
	double Y; // Y��ǥ
	char name[10]; // �������� �̸�
}Button;

typedef struct _player { // �÷��̾��� ������ �����ϴ� ����ü
	Point Loc; // ��ȯ�� �÷��̾� ��ġ ��ǥ
	Point TLoc; // ��ȯ�� �÷��̾� ��ġ ��ǥ
	double SlowTimer; // ���� ��ȭ�ð�
	double SlowStart; // ��ȭ�� ���۵� �ð�
	int Slowflag; // ��ȭ ���� (0:����, 1:��ȭ �ɸ�)
	double size; // �÷��̾��� ũ��
	double Time; // �����ð�
	int Coin; // ���� ������ ����
	int hint; // ���� ��Ʈ ���� (1: ������, 0:�����)
	double baseX; // �̷θ� �׸��� ������ X��ǥ
	double baseY; // �̷θ� �׸��� ������ Y��ǥ
	double defaultX; // ù DrawMaze����� ���� baseX (ȭ��ũ�� ������)
	double defaultY; // ù DrawMaze����� ���� baseY (ȭ��ũ�� ������)
	double L; // �̷� �� ĭ�� ����
	double zoom; // �̷θ� Ȯ���� ����
	double speed; // �÷��̾� �̵��ӵ�
	double slow_speed; // �÷��̾� ��ȭ�� �̵��ӵ�
	Point Dest; // �������� ��ġ
	double hintTimer; // ���� ��Ʈ��� �ð�
	double StartTime; // �������� ���� �ð�
	double hintStartTime; // ��Ʈ��� ���۽ð�
	Point CoinDest[3]; // ������ ��ġ �迭
	int GetCoin[3]; // �� ������ �Ծ������� ���� (0:�ȸ���, 1:����)
	Block Move; // �̵��� ������ ���� �迭(UDLR����)
	Block SlowMove; // ��ȭ�� �̵��� ������ ���� �迭(UDLR����)
}Player;

void SetCoinLoc(int Num, int Limit); // ������ ��ġ�� �缳����
void SetStageSetting(int Num, double Zoom); // ���������� ������ �ʱ�ȭ��
void StageReset(int Num); // ���������� �ʱ�ȭ��
void update_file(); // ���̺� �����͸� ���Ϸ� ����ϴ� �Լ�
void Ensnare(); // ��ȭ ���¸� �����ϴ� �Լ�