#include<stdio.h>
#include<string.h>
#include<windows.h>
#include<time.h>
#include<conio.h>
#include"stdlib.h"
/*
�� 224 72
�� 224 80
�� 224 75
�� 224 77
*/
const WORD FORE_BLUE   = FOREGROUND_BLUE;           //��ɫ�ı�����
const WORD FORE_GREEN  = FOREGROUND_GREEN;          //��ɫ�ı�����
const WORD FORE_RED    = FOREGROUND_RED;            //��ɫ�ı�����
const WORD FORE_PURPLE = FORE_BLUE | FORE_RED;      //��ɫ�ı�����
const WORD FORE_CYAN   = FORE_BLUE | FORE_GREEN;    //��ɫ�ı�����
const WORD FORE_YELLOW = FORE_RED | FORE_GREEN;     //��ɫ�ı�����
const WORD FORE_GRAY   = FOREGROUND_INTENSITY;      //��ɫ�ı�����

const WORD BACK_BLUE   = BACKGROUND_BLUE;           //��ɫ��������
const WORD BACK_GREEN  = BACKGROUND_GREEN;          //��ɫ��������
const WORD BACK_RED    = BACKGROUND_RED;            //��ɫ��������
const WORD BACK_PURPLE = BACK_BLUE | BACK_RED;      //��ɫ��������
const WORD BACK_CYAN   = BACK_BLUE | BACK_GREEN;    //��ɫ��������
const WORD BACK_YELLOW = BACK_RED | BACK_GREEN;     //��ɫ��������
const WORD BACK_GRAY   = BACKGROUND_INTENSITY;      //��ɫ��������
HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //��ñ�׼����豸���
CONSOLE_SCREEN_BUFFER_INFO csbi;                        //���崰�ڻ�������Ϣ�ṹ��
// �ߵ�����ڵ�
struct BodyNode {
	int typeFlag; // ���ͣ���������ͷ�����塢β��
	int x; // ������ͼ�ϵ�x����
	int y; // ������ͼ�ϵ�y����
	BodyNode *next; // ��һ���ڵ�
	BodyNode *last; // ��һ���ڵ� 
};
// ͷ�ڵ�
BodyNode* head;
// β�� 
BodyNode* tail; 
// �ߵ��ƶ�����(1:�ϡ�2:�ҡ�3:�¡�4:��5:��ֹ״̬)
int direction = 5; 
// ��Ϸ״̬��1�������С�0����������ͣ�� 
int gameStatus = 1; 
// ����״̬��1���������С�0��������
int power = 1; 
// �������Ƿ񴴽�ʳ��
bool createFoodFlag = false;
// ��ǰ��С�ߵĳ���(����ͷ)
int snakeLength = 3;
int snakeSpeed = 300;
const int GAME_MAP_HEIGHT = 25;
const int GAME_MAP_WIDTH = 25;
// [y:��][x:��] 
int GAME_MAP[GAME_MAP_HEIGHT][GAME_MAP_WIDTH];
/*
width : �У� һ���еĳ��ȵ��������еĳ��ȣ�����δ�������������width��ԭֵ��+1 
height : �� 
*/
void resetWindowSize(int width, int height) {
	char cmd[300];
	sprintf(cmd,"mode con lines=%d cols=%d ",  height, width);
    system(cmd);
}
// ��Ϸ���ݳ�ʼ�� 
void init() {
	// ��ʼ����ͼ����
	for (int i=0; i<GAME_MAP_HEIGHT; i++) {
		for (int j=0; j<GAME_MAP_WIDTH; j++) {
			int t = 0;
			if (0 == i || (GAME_MAP_HEIGHT -1) == i || (GAME_MAP_WIDTH - 1) == j || 0 == j) {
				t = 1;
			}
			GAME_MAP[i][j] = t;
		}
	}
	// Ͷ��ʳ��
	// GAME_MAP[8][3] = 8;
	// GAME_MAP[2][6] = 8;
	// ��ʼ���ߵ�����
	head = (BodyNode*)malloc(sizeof(BodyNode));
	head->x = 5;
	head->y = 5;
	head->last = NULL; 
	head->typeFlag = 4;
	GAME_MAP[head->y][head->x] = head->typeFlag;
	BodyNode* node1 = (BodyNode*)malloc(sizeof(BodyNode));
	node1->x = 4; // �� 
	node1->y = 5;
	node1->last = head;
	node1->typeFlag = 3;
	GAME_MAP[node1->y][node1->x] = node1->typeFlag;
	head->next = node1;
	BodyNode*  node2 = (BodyNode*)malloc(sizeof(BodyNode));
	node2->x = 3;
	node2->y = 5;
	node2->next = NULL;
	node2->last = node1;
	node2->typeFlag = 3;
	GAME_MAP[node2->y][node2->x] = node2->typeFlag;
	node1->next = node2;
	tail = node2; 

}
// ������л����ı���ָ����λ��(y�����У�x������)
void toPosition(int x, int y) {
    // ���¹��λ�� 
    COORD pos;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(hOutput, pos);
    // ���ع�� 
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = sizeof(cursor);
    SetConsoleCursorInfo(hOutput, &cursor);
}
// ������л�����ͼ��ָ��λ�� 
void toMapSite(int x, int y) {
	toPosition(x * 2 + 1, y);
}
// ��ӡ��ʶ���� 
void outputFlag(int flag) {
	SetConsoleTextAttribute(handle_out, BACK_YELLOW | FORE_GREEN | FORE_BLUE | FORE_RED);
	switch(flag) {
		case 0 : printf("  "); break;
		case 1 : printf("��"); break;
		case 2 : printf("��"); break;
		case 3 : 
			SetConsoleTextAttribute(handle_out, FORE_RED | BACK_YELLOW);
			printf("��"); break;
		case 4 : 
			SetConsoleTextAttribute(handle_out, FORE_RED | BACK_YELLOW);
			printf("��"); break;
		case 5 : printf("��"); break;
		case 6 : printf("��"); break;
		case 7 : 
			SetConsoleTextAttribute(handle_out, FORE_RED | BACK_YELLOW);
			printf("��"); break;
		case 8 : 
			SetConsoleTextAttribute(handle_out, FORE_BLUE | BACK_YELLOW);
			printf("��"); break;
	}
}

// ---------------------------- �����صĵ���
// ��ӡ��ǰ��ͼ�ϵ����ݣ��ߵ�λ�á�ǽ�ڡ�ʳ�
void printMap() {
	for (int i=0; i<GAME_MAP_HEIGHT; i++) {
		for (int j=0; j<GAME_MAP_WIDTH; j++) {
			int t = GAME_MAP[i][j];
			toMapSite(j, i); 
			outputFlag(t);
		}
		puts("");
	}
}
// С�ߵ��ƶ����� 
void moving(int x, int y) {
	if (GAME_MAP[y][x] == 8) {
		// �����ʳ��򴥷���ʳ���� 
		BodyNode* node = (BodyNode*)malloc(sizeof(BodyNode));
		node->typeFlag = 3;
		node->x = head->x;
		node->y = head->y;
		head->next->last = node;
		node->next = head->next;
		node->last = head;
		head->next = node;
		head->x = x;
		head->y = y;
		toMapSite(node->x, node->y);
		outputFlag(3);
		toMapSite(head->x, head->y);
		outputFlag(head->typeFlag);
		// ����ʳ��
		createFoodFlag = true;
		// ���ȼ�һ
		snakeLength ++;
		// �ٶȼӿ�
		snakeSpeed -= 3;
		if (snakeSpeed < 0) {
			snakeSpeed = 0;
		}
		// ���³�����ʾ
		SetConsoleTextAttribute(handle_out, FORE_YELLOW);
		toMapSite(0, GAME_MAP_HEIGHT);
		printf("[��ǰС�ߵĳ��� %d]", snakeLength); 
		return;
	}
	if (GAME_MAP[y][x] == 1 || GAME_MAP[y][x] == 3) {
		// ���ײǽ����ҧ���Լ����������Ϸ 
		gameStatus = 0;
	} 
		// ��ָ�������ƶ�һ��
	GAME_MAP[head->y][head->x] = 3;
	toMapSite(head->x, head->y);
	outputFlag(3);
	BodyNode* temp;
	GAME_MAP[tail->y][tail->x] = 0;
	//toPosition(0, 0);
	//printf("ִ��%d, %d", tail->x, tail->y); 
	toMapSite(tail->x, tail->y);
	outputFlag(0);
	tail->x = head->x;
	tail->y = head->y;
	temp = tail->last;
	temp->next = NULL;
	head->next->last = tail;
	tail->next = head->next;
	tail->last = head;
	head->next = tail;
	head->x = x;
	head->y = y;
	tail = temp;
	// ���»���ͼ��
	GAME_MAP[head->y][head->x] = head->typeFlag;
	toMapSite(head->x, head->y);
	if (gameStatus == 0) {
		outputFlag(7);
	} else {
		outputFlag(head->typeFlag);
	}
	
	
}
void refresh() {
	// (1:�ϡ�2:�ҡ�3:�¡�4:��5:��ֹ״̬)
	switch(direction) {
	case 1 : 
		moving(head->x, head->y - 1);
		break;
	case 2 : 
		moving(head->x + 1, head->y);
		break;
	case 3 : 
		moving(head->x, head->y + 1);
		break;
	case 4 : 
		moving(head->x - 1, head->y);
		break;
	}
}
// ���Ͷ��ָ��������ʳ�� 
void createFood(int number) {
	for(int i=0; i<number; i++) {
		int y = rand() % GAME_MAP_HEIGHT;
		int x = rand() % GAME_MAP_WIDTH;
		// y = GAME_MAP_HEIGHT - 2;
		// x = GAME_MAP_WIDTH - 2;
		int tX = x;
		int tY = y;
		bool validResult = true;
		while(GAME_MAP[y][x] != 0) {
			x++;
			if (x >= GAME_MAP_WIDTH) {
				y += 1;
				x = 0;
			}
			if (y >= GAME_MAP_HEIGHT) {
				y = 0;
			}
			if (tX == x && tY == y) {
				validResult = false;
				break;
			}
		}
		if (validResult) {
			GAME_MAP[y][x] = 8;
			toMapSite(x, y);
			outputFlag(8);
		}
	}
}
// ��Ϸ�߼����к��� 
void gameEngine() {
	// ��Ϸ���ݳ�ʼ�� 
	init();
	// ����Ӧ�ô��ڴ�С 
	resetWindowSize(GAME_MAP_WIDTH * 2 + 2, GAME_MAP_HEIGHT + 3); 
	// ��ӡ��Ϸ��ͼ 
	printMap();
	// Ͷ��ʳ�� 
	createFood(5);
	SetConsoleTextAttribute(handle_out, FORE_YELLOW);
	toPosition(0, GAME_MAP_HEIGHT);
	printf("[��ǰС�ߵĳ��� %d]", snakeLength); 
	createFoodFlag = true;
	// ��Ϸ��ʼʱ���ܾ���ʻ�ķ���
	int startRefuseDirection = 4;
	while(gameStatus != 0) {
		if (createFoodFlag) {
			createFood(1);
			createFoodFlag = false;
		}
		if(_kbhit()) {
			int input = _getch();
			int input2 = _getch();
			switch(input2) {
				case 72 : direction = (direction == 3 ? 3 : 1); break;
				case 80 : direction = (direction == 1 ? 1 : 3); break;
				case 75 : direction = (direction == 2 ? 2 : 4); break;
				case 77 : direction = (direction == 4 ? 4 : 2); break;
				default : direction = 5;
			}
			if (startRefuseDirection != 0 && startRefuseDirection == direction) {
				direction = 5;
				continue;
			} else {
				startRefuseDirection = 0;
			}
		}
		refresh();
		Sleep(snakeSpeed);
	}
	SetConsoleTextAttribute(handle_out, FORE_RED);
	toPosition(0, GAME_MAP_HEIGHT+1);
	printf("��Ϸ�ѽ�����С�ߵĳ��ȴﵽ�˾��˵� %d\n", snakeLength); 
	SetConsoleTextAttribute(handle_out, FORE_GREEN);
	system("pause");
}
// �������� 
void printPage_HELP() {
	resetWindowSize( 58, 13); 
	puts("");
	puts(" ��������������������������������������������������������");
	puts(" ��                                                    ��"); 
	puts(" ��  >_��ҳ�µĲ�����ʽ��                              ��");
	puts(" ��  ���̵����°�ť���ƹ���ƶ�������س���ȷ��ѡ�  ��");
	puts(" ��                                                    ��"); 
	puts(" ��  >_ ��Ϸ����                                     ��"); 
	puts(" ��  ͨ�����̵��������Ҽ�����С�ߵ��ƶ���              ��");
	puts(" ��  �Ե�Խ��ʳ�С�߽�Խ�������ٶ�Ҳ��Խ�죡        ��");
	puts(" ��  ��С��ײ�����Լ�����ײ����ǽ��ʱ����������        ��"); 
	puts(" ��                                  �����������      ��"); 
	puts(" ��������������������������������������������������������");
	toMapSite(16, 10);
	system("pause");
}
// ��ʷ���� 
void printPage_HISTORY() {
	resetWindowSize( 58, 13); 
	puts("");
	puts(" ��������������������������������������������������������");
	puts(" ��                                                    ��"); 
	puts(" ��                                                    ��");
	puts(" ��                                                    ��"); 
	puts(" ��           �� �� �� ...                             ��"); 
	puts(" ��                                                    ��"); 
	puts(" ��                                                    ��"); 
	puts(" ��                                                    ��"); 
	puts(" ��                                                    ��"); 
	puts(" ��                                                    ��");  
	puts(" ��������������������������������������������������������");
	toMapSite(16, 10);
	system("pause");
}
// ��ҳ���ӻ��� 
void printPage_HOME_selectAction(int oldSelect, int newSelect) {
	// toPosition(0, 0);
	// printf("%d, %d", oldSelect, newSelect);
	toMapSite(8, 4 + oldSelect);
	if (0 == oldSelect) {
		printf("  ��ʼ��Ϸ  "); 
	} else if (1 == oldSelect) {
		printf("  ��ʷ��¼  ");
	} else if (2 == oldSelect) {
		printf("  �˳���Ϸ  ");
	}
	toMapSite(8, 4 + newSelect);
	if (0 == newSelect) {
		printf("==> ��ʼ��Ϸ"); 
	} else if (1 == newSelect) {
		printf("==> ��ʷ��¼");
	} else if (2 == newSelect) {
		printf("==> �˳���Ϸ");
	}
}
// ��ҳ 
void printPage_HOME() {
	system("color 0f");
	resetWindowSize( 32, 9); 
	int select = 0, t = select;
	puts("                               ");
	puts(" ������������������������������");
	puts(" ��  ���� ��  CODE   SNAKE ��");
	puts(" ��  ��                      ��");
	puts(" ��  ��          ==> ��ʼ��Ϸ��"); 
	puts(" ��               ��ʷ��¼   ��"); 
	puts(" ��     ��        �˳���Ϸ   ��");
	puts(" ������������������������������");
	toMapSite(10, 3);
	bool ac = true;
	while(ac) {
		if(_kbhit()) {
			int input = _getch();
			// toPosition(0, 0);
			// printf("%d", input);
			if (13 == input) {
				break;
			} else if(224 != input) {
				select = -1;
				break;
			}
			int input2 = _getch();
			switch(input2) {
				// �� 
				case 72 : 
					t = select;
					select -= 1;
					select = select < 0 ? 2 : select; 
					printPage_HOME_selectAction(t, select);
					break; 
				// �� 
				case 80 : 
					t = select;
					select += 1;
					select = select > 2 ? 0 : select; 
					printPage_HOME_selectAction(t, select);
					break;
				default : 
					select = -1;
					ac = false;
					continue;
					break;
					;
			}
		}
	}
	switch(select) {
		case 0 : 
			gameEngine();  
			break;
		case 1 : 
			printPage_HISTORY();
			break;
		case 2 : 
			power = 0;
			break;
		case -1 : 
			printPage_HELP();
			break;
	}
}
// ������� 
int main() {
	system("title codeSNAKE"); // �޸Ĵ��ڱ��� 
	GetConsoleScreenBufferInfo(handle_out, &csbi);          //��ô��ڻ�������Ϣ
	while(power) {
		printPage_HOME();
	}
	return 0;
}
