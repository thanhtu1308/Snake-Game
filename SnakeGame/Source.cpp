#include <stdio.h>
#include<windows.h>
#include<iostream>
#include <string>
#include<thread>
#include<conio.h>
#include<time.h>


#pragma comment(lib, "winmm.lib")

#define _CRT_SECURE_NO_DEPRECATE
using namespace std;

//B3
//Hằng số
#define MAX_SIZE_SNAKE 15
#define MAX_SIZE_FOOD 4
#define MAX_SPEED 3
// Biến toàn cục
POINT snake[12]; //Con rắn
POINT food[4]; // Thức ăn
/******************************************/
POINT gate[5] ;
int temp;
/*******************************************/

int CHAR_LOCK;//Biến xác định hướng không thể di chuyển (Ở một thời điểm có một hướng rắn không thể di chuyển)
int MOVING;//Biến xác định hướng di chuyển của snake (Ở một thời điểm có ba hướng rắn có thể di chuyển)
int SPEED;// Có thể hiểu như level, level càng cao thì tốc độ càng nhanh
int HEIGH_CONSOLE, WIDTH_CONSOLE;// Độ rộng và độ cao của màn hình console
int FOOD_INDEX; // Chỉ số food hiện hành đang có trên màn hình
int SIZE_SNAKE; // Kích thước của snake, lúc đầu có 7 theo mã số sinh viên
int STATE; // Trạng thái sống hay chết của rắn

char LengthSnake[] = { '1','5','6','0','6','5','5' };

//B1
void FixConsoleWindow()
{
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
	system("mode 120,33");
}
//B2
void GotoXY(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

}


//B4
//Thiết lập dữ kiệu về trang thái ban đầu

bool IsValid(int x, int y)
{
	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		if (snake[i].x == x && snake[i].y == y)
		{
			return false;
		}
		return true;
	}
}
////***************
void GenerateFood()
{
	int x, y;
	srand(time(NULL));
	for (int i = 0; i < MAX_SIZE_FOOD; i++)
	{
		do
		{
			x = rand() % (WIDTH_CONSOLE - 1) + 1;   
			y = rand() % (HEIGH_CONSOLE - 1) + 1;  
		} while (IsValid(x, y) == false);
		food[i] = { x,y };

	}
}
void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
	/// thay doi mau giao dien 4 buc tuong X
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 10);

	GotoXY(x, y);
	cout << 'X';

	for (int i = 1; i < width; i++)cout << 'X';
	cout << 'X';

	GotoXY(x, height + y); cout << 'X';

	for (int i = 1; i < width; i++)cout << 'X';
	cout << 'X';

	for (int i = y + 1; i < height + y; i++)
	{
		GotoXY(x, i); cout << 'X';
		GotoXY(x + width, i); cout << 'X';
	}
	GotoXY(curPosX, curPosY);
}

void drawMenu()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 4); // 12: màu text 
	GotoXY(0, HEIGH_CONSOLE + 1);
	for (int i = 0; i < WIDTH_CONSOLE + 1; i++)
	{
		cout << 'X';
	}
	GotoXY(0, HEIGH_CONSOLE + 12);
	for (int i = 0; i < WIDTH_CONSOLE + 1; i++)
	{
		cout << 'X';
	}
	for (int i = HEIGH_CONSOLE + 2; i < HEIGH_CONSOLE + 12; i++)
	{
		GotoXY(0, i);
		cout << 'X';
		GotoXY(70, i);
		cout << 'X';
	}

	GotoXY(30, 23);
	//SetConsoleTextAttribute(consoleHandle, 5);
	cout << "[Menu Game]";

	GotoXY(27, 26);
	cout << " Pause: >>  P  <<";
	GotoXY(27, 27);
	cout << " Save:  >>  L  <<";
	GotoXY(27, 28);
	cout << " Upload:>>  T  <<";
	GotoXY(27, 29);
	cout << " Exit:  >> Esc <<";
	GotoXY(0, 0);
}
void drawInfoUI()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 7);
	GotoXY(WIDTH_CONSOLE + 1, 0);
	for (int i = 1; i < 120 - WIDTH_CONSOLE; i++) cout << '=';

	GotoXY(WIDTH_CONSOLE + 1, HEIGH_CONSOLE + 12);

	for (int i = 1; i < 120 - WIDTH_CONSOLE; i++)
	{
		cout << '=';
	}
	for (int i = 1; i < HEIGH_CONSOLE + 12; i++)
	{
		GotoXY(WIDTH_CONSOLE + 1, i); cout << '|';
		GotoXY(119, i); cout << '|';
	}
	
	GotoXY(WIDTH_CONSOLE + 13, 3);
	cout << "WELCOME TO SNAKE GAME";




	GotoXY(0, 0);
}

void ResetData()

{
	//Khởi tạo các giá trị toàn cục
	CHAR_LOCK = 'A', MOVING = 'D', SPEED = 1; 
	FOOD_INDEX = 0, WIDTH_CONSOLE = 70, HEIGH_CONSOLE = 20, SIZE_SNAKE = 7; 
	//Khởi tạo giá trị mặc định cho snake
	snake[0] = { 10, 5 }; snake[1] = { 11, 5 };
	snake[2] = { 12, 5 }; snake[3] = { 13, 5 };
	snake[4] = { 14, 5 }; snake[5] = { 15, 5 };
	//thêm toạ dộ snake thứ 7
	snake[6] = { 16, 5 };
	GenerateFood();//Tạo mảng thức ăn food
}


void DrawFood()
{
	GotoXY(food[FOOD_INDEX].x, food[FOOD_INDEX].y);
	printf("*");
}
void DrawSnake(char* s)
{

	int j = 0;
	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		GotoXY(snake[i].x, snake[i].y);
		printf("%c", s[j++]);
		if (j > 6)
		{
			j = 0;
		}
	}
}
//tao cong qua mang choi
void CreateGate() 
{
	int x, y;
	srand(time(NULL));
	do {
		x =3 + rand() % (WIDTH_CONSOLE + 1 - 10) ;// 3 - >
		y =3 + rand() % (HEIGH_CONSOLE + 1 - 5); //3 ->
	} while (IsValid(x, y) == false);
	gate[0] = { x - 1, y + 1 };
	gate[1] = { x - 1, y };
	
	gate[2] = { x, y };
	gate[3] = { x + 1, y };
	gate[4] = { x + 1, y + 1 };
	gate[5] = { x, y + 1 };
	
}
void DrawGate()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 12);
	for (int i = 0; i < 5; i++) {
		GotoXY(gate[i].x, gate[i].y);
		cout << "o";
	}
}
void CleanGate()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 12);
	for (int i = 0; i < 5; i++) {
		GotoXY(gate[i].x, gate[i].y);
		cout << " ";
		gate[i].x = 0; gate[i].y = 0;
	}
}

int MenuStart() {
	//STATE = 0;
	GotoXY(2, 5); // gắn vị trí ký tự đầu tiên của câu thông báo
	printf("Type 't' to play from file or any key to new game");
	GotoXY(0, 0); // gán lại vị trí nếu muốn ghi ký tự ra ngoài màn hình
	
	return toupper(_getch()); // trả về ký tự đã nhập
}
void StartGame()
{
	system("cls");
	ResetData(); // Khởi tạo dữ liệu gốc
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE); // Vẽ màn hình game
	drawMenu();
	drawInfoUI();
	STATE = 1;//Bắt đầu cho Thread chạy
	GotoXY(0, 0);
}


//B6

//Hàm thoát game
void ExitGame(HANDLE t) {
	system("cls");
	exit(0);
	TerminateThread(t, 0);
}

//Hàm dừng game
void PauseGame(HANDLE t)
{
	SuspendThread(t);
	
}
void SaveGame()
{
	char nameFile[20];

	GotoXY(WIDTH_CONSOLE + 13, 10);
	cout << "---------------------------";
	GotoXY(WIDTH_CONSOLE + 13, 11);
	cout << "        SAVE  GAME         ";
	GotoXY(WIDTH_CONSOLE + 13, 12);
	cout << "---------------------------";
	GotoXY(WIDTH_CONSOLE + 13, 15);
	cout << "---------------------------";
	GotoXY(WIDTH_CONSOLE + 13, 16);
	cout << "| File Name:               |";
	GotoXY(WIDTH_CONSOLE + 13, 17);
	cout << "---------------------------";
	GotoXY(WIDTH_CONSOLE + 25, 16);
	gets_s(nameFile);
	strcat_s(nameFile, ".txt"); // noi chuoi ten file va .txt
	FILE *f = fopen(nameFile, "wt");

	fprintf(f, "%d\n", FOOD_INDEX);
	fprintf(f, "%d %d\n", food[FOOD_INDEX].x, food[FOOD_INDEX].y);
	fprintf(f, "%d\n", SPEED);
	fprintf(f, "%d\n", SIZE_SNAKE);
	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		fprintf(f, "%d %d\n", snake[i].x, snake[i].y);
	}	
	for (int i = 0; i < 5; i++)
	{
		fprintf(f, "%d %d\n", gate[i].x, gate[i].y);
	}	
	fclose(f);
	GotoXY(0, 0);
}
void LoadGame()
{
	char nameFile[20];
	char space[30];
	for (int i = 0; i < 19; i++)
		space[i] = 32;
	DrawSnake(space);
	GotoXY(food[FOOD_INDEX].x, food[FOOD_INDEX].y); 
	cout << " ";
	GotoXY(WIDTH_CONSOLE + 13, 10);
	cout << "---------------------------";
	GotoXY(WIDTH_CONSOLE + 13, 11);
	cout << "        LOAD  GAME         ";
	GotoXY(WIDTH_CONSOLE + 13, 12);
	cout << "---------------------------";
	GotoXY(WIDTH_CONSOLE + 13, 15);
	cout << "---------------------------";
	GotoXY(WIDTH_CONSOLE + 13, 16);
	cout << "| File Name:               |";
	GotoXY(WIDTH_CONSOLE + 13, 17);
	cout << "---------------------------";
	GotoXY(WIDTH_CONSOLE + 25, 16);
	gets_s(nameFile);
	strcat_s(nameFile, ".txt"); // noi chuoi ten file va .txt
	FILE *f = fopen(nameFile, "rt"); //
	if (f == NULL)
	{
		GotoXY(WIDTH_CONSOLE + 13, 19);
		cout << "File khong ton tai !!!";
		_getch();
	}
	fseek(f, 0, 0L);
	fscanf(f, "%d\n", &FOOD_INDEX);
	fscanf(f, "%d %d\n", &food[FOOD_INDEX].x, &food[FOOD_INDEX].y);
	fscanf(f, "%d\n", &SPEED);
	fscanf(f, "%d\n", &SIZE_SNAKE);

	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		fprintf(f, "%d %d\n", snake[i].x, snake[i].y);
	}
	for (int i = 0; i < 5; i++)
	{
		fprintf(f, "%d %d\n", gate[i].x, gate[i].y);
	}
	fclose(f);
}
void StartLoad()
{
	system("cls");
	ResetData();
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE); // Vẽ màn hình game
	drawMenu();
	drawInfoUI();
	LoadGame();
	DrawSnake(LengthSnake);
	DrawFood();
	
	STATE = 1;//Bắt đầu cho Thread chạy
}



//B7
//xử lý khi “snake” ăn “food”
//Hàm cập nhật dữ liệu toàn cục
//co thay doi/********************ST********************/
void Eat()
{
	snake[SIZE_SNAKE] = food[FOOD_INDEX];
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1)
	{
		temp = SIZE_SNAKE;
		FOOD_INDEX = 0;
		
		if (SPEED == MAX_SPEED)
			SPEED = 1;
		else
			SPEED++;
		CreateGate();
		DrawGate();
	}
	else 
	{
		FOOD_INDEX++;
		SIZE_SNAKE++;
	}
}
//B8
//xử lý khi snake va chạm với tường rào xung quanh
//Hàm xử lý khi snake chết
void ProcessDead()
{
	STATE = 0;
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 4);
	GotoXY(15, HEIGH_CONSOLE / 2);
	printf("Dead, type y to continue or anykey to exit");
	PlaySound(TEXT("game-over.wav"), NULL, SND_ASYNC);
}
//B9
//Hàm vẽ màn hình snake



//B10
//Xu li cac huong di chuyen
void MoveRight()
{
	/**/
	// xử lý va chạm với cổng vào để qua màn
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1) //
	{
		for (int i = 0; i < 5; i++) 
		{
			if (gate[i].x == snake[SIZE_SNAKE - 1].x&&gate[i].y == snake[SIZE_SNAKE - 1].y) 
			{
				ProcessDead(); 
				break;
			}
		}
	}
	// xử lý va chạm với thân snake
	for (int i = 0; i < SIZE_SNAKE - 2; i++) 
	{
		if (snake[i].x == snake[SIZE_SNAKE - 1].x + 1 && snake[i].y == snake[SIZE_SNAKE - 1].y) 
		{
			ProcessDead();
			break;
		}
	}
	/**/
	//xử lý va chạm với bức tường 
	if (snake[SIZE_SNAKE - 1].x + 1 == WIDTH_CONSOLE)
	{
		ProcessDead();
	}
	else
	{
		if (snake[SIZE_SNAKE - 1].x + 1 == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y == food[FOOD_INDEX].y)
		{
			Eat();
		}
		for (int i = 0; i < SIZE_SNAKE - 1; i++)
		{
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;
		}
		snake[SIZE_SNAKE - 1].x++;

	}
}

void MoveLeft()
{
	/**/
	// xử lý va chạm với cổng vào để qua màn
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1) {
		for (int i = 0; i < 5; i++) {
			if (gate[i].x == snake[SIZE_SNAKE - 1].x&&gate[i].y == snake[SIZE_SNAKE - 1].y) {
				ProcessDead(); break;
			}
		}
	}
	// xử lý va chạm với thân snake
	for (int i = 0; i < SIZE_SNAKE - 2; i++) {
		if (snake[i].x == snake[SIZE_SNAKE - 1].x - 1 && snake[i].y == snake[SIZE_SNAKE - 1].y) {
			ProcessDead();
			break;
		}
	}
	/**/
	//xử lý va chạm với bức tường 
	if (snake[SIZE_SNAKE - 1].x - 1 == 0)
	{
		ProcessDead();

	}
	else
	{
		if (snake[SIZE_SNAKE - 1].x - 1 == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y == food[FOOD_INDEX].y)
		{
			Eat();
		}
		for (int i = 0; i < SIZE_SNAKE - 1; i++)
		{
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;
		}
		snake[SIZE_SNAKE - 1].x--;
	}
}

void MoveDown()
{
	/**/
	//xử lý va chạm với cổng vào để qua màn
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1) {
		for (int i = 0; i < 5; i++) {
			if (gate[i].x == snake[SIZE_SNAKE - 1].x&&gate[i].y == snake[SIZE_SNAKE - 1].y) {
				ProcessDead(); break;
			}
		}
	}
	// xử lý va chạm với thân snake
	for (int i = 0; i < SIZE_SNAKE - 2; i++) {
		if (snake[i].x == snake[SIZE_SNAKE - 1].x && snake[i].y == snake[SIZE_SNAKE - 1].y + 1) {
			ProcessDead();
			break;
		}
	}
	/**/
	//xử lý va chạm với bức tường 
	if (snake[SIZE_SNAKE - 1].y + 1 == HEIGH_CONSOLE)
	{
		ProcessDead();
	}
	else
	{
		if (snake[SIZE_SNAKE - 1].x == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y + 1 == food[FOOD_INDEX].y)
		{
			Eat();
		}
		for (int i = 0; i < SIZE_SNAKE - 1; i++)
		{
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;

		}
		snake[SIZE_SNAKE - 1].y++;
	}
}

void MoveUp()
{
	/**/
	
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1) 
	{
		for (int i = 0; i < 5; i++) 
		{
			
			if (gate[i].x == snake[SIZE_SNAKE - 1].x&&gate[i].y == snake[SIZE_SNAKE - 1].y)
			{
				ProcessDead();
				break;
			}
		}
		
		if (snake[SIZE_SNAKE - 1].x == gate[5].x&&snake[SIZE_SNAKE - 1].y == gate[5].y)
		{
			SIZE_SNAKE--;
		}
		if (SIZE_SNAKE == 0) 
		{
			SPEED++;
			if (SPEED == MAX_SPEED)
			{
				CleanGate();
				ResetData();
			}
			else {
				FOOD_INDEX = 0;
				SIZE_SNAKE = temp++;
				CleanGate();
				
			}
		}
	}
	/**/
	////xử lý va chạm với bức tường 
	for (int i = 0; i < SIZE_SNAKE - 2; i++) {
		if (snake[i].x == snake[SIZE_SNAKE - 1].x && snake[i].y == snake[SIZE_SNAKE - 1].y - 1) {
			ProcessDead();
			break;
		}
	}
	if (snake[SIZE_SNAKE - 1].y - 1 == 0)
	{
		ProcessDead();
	}
	else {
		if (snake[SIZE_SNAKE - 1].x == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y - 1 == food[FOOD_INDEX].y)
		{
			Eat();
		}
		for (int i = 0; i < SIZE_SNAKE - 1; i++) {
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;
		}
		snake[SIZE_SNAKE - 1].y--;
	}
}
//B11
//Thủ tục cho thread


void ThreadFunc()
{
	while (1) {
		if (STATE == 1)
		{
			//Nếu vẫn còn snake vẫn còn sống
			char spacename[30];
			for (int i = 0; i < 30; i++)
				spacename[i] = 32; // khoang trang
			DrawSnake(spacename);
			/**/
			if (gate[0].x == 0 && gate[0].y == 0)
				DrawFood();
			/**/
			switch (MOVING)
			{
			case 'A':
				MoveLeft();
				break;
			case 'D':
				MoveRight();
				break;
			case 'W':
				MoveUp();
				break;
			case 'S':
				MoveDown();
				break;
			}
			DrawSnake(LengthSnake);
			Sleep(150 / SPEED);//Hàm ngủ theo tốc độ SPEEED
		}
	}
}


void main()
{

	int temp;
	FixConsoleWindow();
	

	thread t1(ThreadFunc); //Tao thread cho snake
	HANDLE handle_t1 = t1.native_handle(); //Lay handle cua thread
	while (1)
	{
		if (MenuStart() == 'T')
		{	
			StartLoad();
		}
		else
		{
			StartGame();
		}
		while (1)
		{


			temp = toupper(_getch());
			if (STATE == 1)
			{

				if (temp == 'P')
				{
					PauseGame(handle_t1);
				}
				else if (temp == 'T')
				{
					PauseGame(handle_t1);
					LoadGame();

				}
				else if (temp == 'L')
				{
					PauseGame(handle_t1); //pause rồi chơi tiếp thôi
					SaveGame();

				}
				else if (temp == 27)
				{
					ExitGame(handle_t1);
					return;
				}
				else
				{
					ResumeThread(handle_t1);
					if ((temp != CHAR_LOCK) && (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S'))
					{
						if (temp == 'D')
						{
							CHAR_LOCK = 'A';
						}
						else if (temp == 'W')
						{
							CHAR_LOCK = 'S';
						}
						else if (temp == 'S')
						{
							CHAR_LOCK = 'W';
						}
						else
							CHAR_LOCK = 'D';
						MOVING = temp;
					}

				}


			}
			else
			{
				if (temp == 'Y')
				{
					StartGame();
				}
				else
				{
					ExitGame(handle_t1);
					return;
				}
			}
		}
	}
}
