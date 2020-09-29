#include<stdio.h>
#include<string.h>
#include<windows.h>
#include<time.h>
#include<conio.h>
#include"stdlib.h"
/*
上 224 72
下 224 80
左 224 75
右 224 77
*/
const WORD FORE_BLUE   = FOREGROUND_BLUE;           //蓝色文本属性
const WORD FORE_GREEN  = FOREGROUND_GREEN;          //绿色文本属性
const WORD FORE_RED    = FOREGROUND_RED;            //红色文本属性
const WORD FORE_PURPLE = FORE_BLUE | FORE_RED;      //紫色文本属性
const WORD FORE_CYAN   = FORE_BLUE | FORE_GREEN;    //青色文本属性
const WORD FORE_YELLOW = FORE_RED | FORE_GREEN;     //黄色文本属性
const WORD FORE_GRAY   = FOREGROUND_INTENSITY;      //灰色文本属性

const WORD BACK_BLUE   = BACKGROUND_BLUE;           //蓝色背景属性
const WORD BACK_GREEN  = BACKGROUND_GREEN;          //绿色背景属性
const WORD BACK_RED    = BACKGROUND_RED;            //绿色背景属性
const WORD BACK_PURPLE = BACK_BLUE | BACK_RED;      //紫色背景属性
const WORD BACK_CYAN   = BACK_BLUE | BACK_GREEN;    //青色背景属性
const WORD BACK_YELLOW = BACK_RED | BACK_GREEN;     //黄色背景属性
const WORD BACK_GRAY   = BACKGROUND_INTENSITY;      //灰色背景属性
HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //获得标准输出设备句柄
CONSOLE_SCREEN_BUFFER_INFO csbi;                        //定义窗口缓冲区信息结构体
// 蛇的身体节点
struct BodyNode {
	int typeFlag; // 类型，用于区分头、身体、尾部
	int x; // 所处地图上的x坐标
	int y; // 所处地图上的y坐标
	BodyNode *next; // 下一个节点
	BodyNode *last; // 上一个节点 
};
// 头节点
BodyNode* head;
// 尾巴 
BodyNode* tail; 
// 蛇的移动方向(1:上、2:右、3:下、4:左、5:静止状态)
int direction = 5; 
// 游戏状态（1：运行中、0：死亡、暂停） 
int gameStatus = 1; 
// 控制着是否创建食物
bool createFoodFlag = false;
// 当前的小蛇的长度(包括头)
int snakeLength = 3;
int snakeSpeed = 300;
const int GAME_MAP_HEIGHT = 20;
const int GAME_MAP_WIDTH = 35;
// [y:行][x:列] 
int GAME_MAP[GAME_MAP_HEIGHT][GAME_MAP_WIDTH];

void init() {
	// 初始化地图数据
	for (int i=0; i<GAME_MAP_HEIGHT; i++) {
		for (int j=0; j<GAME_MAP_WIDTH; j++) {
			int t = 0;
			if (0 == i || (GAME_MAP_HEIGHT -1) == i || (GAME_MAP_WIDTH - 1) == j || 0 == j) {
				t = 1;
			}
			GAME_MAP[i][j] = t;
		}
	}
	// 投放食物
	// GAME_MAP[8][3] = 8;
	// GAME_MAP[2][6] = 8;
	// 初始化蛇的数据
	head = (BodyNode*)malloc(sizeof(BodyNode));
	head->x = 5;
	head->y = 5;
	head->last = NULL; 
	head->typeFlag = 4;
	GAME_MAP[head->y][head->x] = head->typeFlag;
	BodyNode* node1 = (BodyNode*)malloc(sizeof(BodyNode));
	node1->x = 4; // 列 
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
// 将光标切换到指定的位置(y代表行，x代表列，并且列需要*2)
void toPosition(int x, int y) {
    // 更新光标位置 
    COORD pos;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    pos.X = x * 2;
    pos.Y = y;
    SetConsoleCursorPosition(hOutput, pos);
    // 隐藏光标 
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = sizeof(cursor);
    SetConsoleCursorInfo(hOutput, &cursor);
}
void outputFlag(int flag) {
	SetConsoleTextAttribute(handle_out, BACK_YELLOW | FORE_GREEN | FORE_BLUE | FORE_RED);
	switch(flag) {
		case 0 : printf("  "); break;
		case 1 : printf("■"); break;
		case 2 : printf("◆"); break;
		case 3 : 
			SetConsoleTextAttribute(handle_out, FORE_RED | BACK_YELLOW);
			printf("◇"); break;
		case 4 : 
			SetConsoleTextAttribute(handle_out, FORE_RED | BACK_YELLOW);
			printf("●"); break;
		case 5 : printf("○"); break;
		case 6 : printf("※"); break;
		case 7 : 
			SetConsoleTextAttribute(handle_out, FORE_RED | BACK_YELLOW);
			printf("＊"); break;
		case 8 : 
			SetConsoleTextAttribute(handle_out, FORE_BLUE | BACK_YELLOW);
			printf("¤"); break;
	}
}

// ---------------------------- 输出相关的调用
// 打印当前地图上的内容（蛇的位置、墙壁、食物）
void printMap() {
	for (int i=0; i<GAME_MAP_HEIGHT; i++) {
		for (int j=0; j<GAME_MAP_WIDTH; j++) {
			int t = GAME_MAP[i][j];
			outputFlag(t);
		}
		puts("");
	}
}
void moving(int x, int y) {
	if (GAME_MAP[y][x] == 8) {
		// 如果是食物，则触发进食机制 
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
		toPosition(node->x, node->y);
		outputFlag(3);
		toPosition(head->x, head->y);
		outputFlag(head->typeFlag);
		// 补充食物
		createFoodFlag = true;
		// 长度加一
		snakeLength ++;
		// 速度加快
		snakeSpeed -= 3;
		if (snakeSpeed < 0) {
			snakeSpeed = 0;
		}
		// 更新长度提示
		SetConsoleTextAttribute(handle_out, FORE_YELLOW);
		toPosition(0, GAME_MAP_HEIGHT);
		printf("[当前小蛇的长度 %d]", snakeLength); 
		return;
	}
	if (GAME_MAP[y][x] == 1 || GAME_MAP[y][x] == 3) {
		// 如果撞墙或者咬到自己，则结束游戏 
		gameStatus = 0;
	} 
		// 向指定方向移动一格
	GAME_MAP[head->y][head->x] = 3;
	toPosition(head->x, head->y);
	outputFlag(3);
	BodyNode* temp;
	GAME_MAP[tail->y][tail->x] = 0;
	//toPosition(0, 0);
	//printf("执行%d, %d", tail->x, tail->y); 
	toPosition(tail->x, tail->y);
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
	// 重新绘制图像
	GAME_MAP[head->y][head->x] = head->typeFlag;
	toPosition(head->x, head->y);
	if (gameStatus == 0) {
		outputFlag(7);
	} else {
		outputFlag(head->typeFlag);
	}
	
	
}
void refresh() {
	// (1:上、2:右、3:下、4:左、5:静止状态)
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
			// toPosition(0, 0);
			// printf("x:%d y:%d", x, y);
			GAME_MAP[y][x] = 8;
			toPosition(x, y);
			outputFlag(8);
		}
	}
}

int main() {
	char cmd[300];
	sprintf(cmd,"mode con lines=%d cols=%d ",  GAME_MAP_HEIGHT + 5, GAME_MAP_WIDTH * 2 + 1);
    system(cmd);
	GetConsoleScreenBufferInfo(handle_out, &csbi);          //获得窗口缓冲区信息
	//toPosition(2, 1);
	init();
	printMap();
	createFood(5);
	//if (true) return 0;
	SetConsoleTextAttribute(handle_out, FORE_YELLOW);
	toPosition(0, GAME_MAP_HEIGHT);
	printf("[当前小蛇的长度 %d]", snakeLength); 
	createFoodFlag = true;
	// 游戏开始时，拒绝行驶的方向
	int startRefuseDirection = 4;

	while(gameStatus != 0) {
		if (createFoodFlag) {
			createFood(1);
			createFoodFlag = false;
		}
		//toPosition(1, 0);
		//puts("等待输入");
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
	printf("游戏已结束，小蛇的长度达到了惊人的 %d\n", snakeLength); 
	SetConsoleTextAttribute(handle_out, FORE_GREEN);
	system("pause");
	return 0;
}
