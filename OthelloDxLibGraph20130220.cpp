
/*
 * othelloAI_Graph.cpp
 *
 *  Created on: 2013-12-5
 *      Author: patriciaxiao
0、下载dxlib：http://homepage2.nifty.com/natupaji/DxLib/dxdload.html
1、文件-》新建项目-》Win32应用程序-》空项目
2、项目-》增加项
3、项目-》项目属性-》
       1）配置-》所有配置-》
                    ①配置属性-》常规-》字符集-》字符集-》使用多字节字符集
                    ②配置属性-》C/C++-》常规-》附加包含目录-》
                                    添加文件夹： \您的解压缩目录\DxLibVCTest\
                    ③配置属性-》链接器-》常规-》附加包含目录-》
                                     同上
       2）配置-》Release-》
                    配置属性-》C/C++-》代码生成-》运行库-》多线程（/MT）
       3）配置-》Debug-》
                    配置属性-》C/C++-》代码生成-》运行库-》多线程调试（/MTd）
*/

// 黑白棋，图形界面的相关定义和函数
#define _CRT_SECURE_NO_WARNINGS
#include "DxLib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> 
#include <time.h>
#include <io.h>
#include <math.h>

#include <iostream>
using namespace std;

// 空位置设为0 ，黑子下的位置设为1 ，白子下的位置设为2
#define EMPTY		'0'
#define BLACK		'1'
#define WHITE	    '2'
#define MARK		'M'
#define SCALE		 8									// 黑白棋规模一般是 8 X 8
#define LOW			 1									// 低级棋力，实际上是搜索深度
#define HIGH		 4									// 高级棋力

#define FileNameLEN  60
#define WaitSomeTime 500								// 控制时间，否则闪得太快

#define inf          0xfffffff							// 最后似乎没派上什么用场？
#define BOARD_SIZE	 36									// 用于控制棋盘占据画面大小的量
#define delta		 6									// 调整棋盘位置的参数
#define CIRCLE_Adj	 3.75						        // 用于根据棋盘格而计算棋子半径的量 	半径 = BOARD_SIZE / CIRCLE_Adj 

// 棋盘初始数据
 
int  BlackColor , WhiteColor ;							// 黑白颜色，黑先下
int  GreenColor , RedColor ;							// 绿色棋盘，红色醒目
int  HumanMark, ComputerMark, MarkPut ;					// 用于标记人、计算机刚翻转颜色
int  HUMAN_Color , COMPUTER_Color ;						// 人选择的颜色。人一般选黑色

int human[SCALE][SCALE]={{0}};
int computer[SCALE][SCALE]={{0}};						// 能否落子

int comp_x = -1, comp_y = -1;							// 最新落子点，初始值定的越界,方便开始的时候提示

char HUMAN=BLACK;										// 纯粹为了存空棋盘的话别出bug
char COMPUTER=WHITE;
int  DEPTH;												// 搜索深度

char board[SCALE][SCALE];
char save_board[SCALE][SCALE];							// 读盘之前放进来处理用的
char mark_board[SCALE][SCALE];							// 用来标记刚被计算机换的颜色
int  MarkTime = 0 ;										// 用于延时观看翻转效果

// "一级菜单：1(新盘) 2(读盘) 3(写盘) ESC(退出)"	
#define MENU1		1
#define MENU2		2
#define Menu_NONE	6
#define Menu1_New	1
#define Menu1_Read	2
#define Menu1_Save	3
#define Menu1_ESC	0
void PrintMenu_1( );									// 打印一级菜单

// "二级菜单：1(求助) 2(棋力) 3(写盘) ESC(退回)"
#define Menu2_Help	1
#define Menu2_Level	2
#define Menu2_Save	3
#define Menu2_ESC	0
void PrintMenu_2( );									// 打印二级菜单

typedef struct box {
	int x1, y1, x2, y2;
} Box; 

Box oldbox = {0, 480, 0, 480};
Box oldboxYN = {0, 480, 0, 480};

void help( );											// 查询可落子的棋盘格
void Level( );											// 选择搜索深度

void change_color(int , int , char, char [SCALE][SCALE], bool);
int judge_put(int ,int , char, char [SCALE][SCALE]);
void possible( );
int mobility(char);
long long find_position(int, char[SCALE][SCALE], long long, long long);

//每个位置的权重
int const_value[SCALE][SCALE];							// 固有部分
int change_value[SCALE][SCALE]={{0}};							// 时时在变

void put_value( );										// 给棋盘格赋权重

void new_board();										// 设置 2 x 2 对阵的初始棋局
bool ReadBoard( );										// 读盘中棋局
int  SaveBoard( );										// 将棋局存盘

#define IllegalChar			1							// 文件中有非法字符 
#define NoCentralChess		2							// 中间没有棋子
#define NotConnected		3							// 未连成一片
#define GoodBoard			0							// 正常棋盘

// 定义键盘选择项的一些常量
#define CHOICE1_YBL			1							// 第一种选择，是1或者Y
#define CHOICE2_NWH			2							// 第二种选择，是2或者N

#define CHOICE_YN			1							// 选择Y或N键
#define CHOICE_BW			2							// 选择B或W键
#define CHOICE_LH			3							// 选择L或H键

int getchoice(int &, int, char[]);						// 根据类型，读取选择值		

void DrawBackground( );									// 画棋盘背景，用于变化比较大的时候
void DrawBoard( );										// 画棋盘中的黑白棋子
void MarkChess( int , int , char );						// 对一个刚换色的棋，缓慢地显示换色过程
void DrawChess( int, int );								// 在(i, j) 棋盘坐标中布置带颜色的棋子
void ColorChess( int , int , int  );					// 给棋子着色，如果用棋盘色则可消除棋子
void DrawText_1( char [], int );						// 在第一行写文字
void DrawText_2( char [], int );						// 在第二行写文字
void DrawText_3( char [], int );						// 在第三行写文字
int  checkMouse(int &, int &, int);						// 把鼠标的点阵位置转换为棋盘格子下标值
void Coordinate(int , int, char [ ], int);				// 把点阵位置转换到字符串中
void PrintScore( );										// 打印玩家和计算机的得分
void choose_Color( );									// 选择棋子颜色，黑色先走（一般都选它）
void start( );											// 程序的主要控制函数
void play( );											// 对弈函数
int  ClickRange(int , int, int);                        // 菜单选择
int  ClickChoice(int ,int, char [] );                   // "1-2" "Y-N"的鼠标选择

char ChoiceMenuStr[ 128 ] = "                                                                   " ;

// 判断棋盘是否初始棋盘
int Is_initialboard( )
{
	int i,j;
	int temp[SCALE][SCALE];

	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++) 
			temp[i][j] = board[i][j];

	if ( temp[SCALE/2-1][SCALE/2-1] != WHITE )
		return false;
	else temp[SCALE/2-1][SCALE/2-1] = EMPTY ;
	if ( temp[SCALE/2][SCALE/2] != WHITE )
		return false;
	else temp[SCALE/2][SCALE/2] = EMPTY ;
	if ( temp[SCALE/2-1][SCALE/2] != BLACK )
		return false;
	else temp[SCALE/2-1][SCALE/2] = EMPTY ;
	if (temp[SCALE/2][SCALE/2-1] != BLACK )
		return false;
	else temp[SCALE/2][SCALE/2-1] = EMPTY ;

	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++) 
			if ( temp[i][j] != EMPTY )
					return false;

	return true;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
						 LPSTR lpCmdLine, int nCmdShow )
{
	SetGraphMode( 640 , 480 , 16 ) ; 					// 可以根据自己的机器分辨率设置，例如SetGraphMode( 1366 , 768 , 32 );

	if( DxLib_Init() == -1 )							// 初始化DxLib 库
	{
		 return -1;										// 若失败则返回-1
	}

	WhiteColor = GetColor( 255 , 255 , 255 ) ;			// 白色
	BlackColor = GetColor( 0 , 0 , 0 );					// 黒色
	GreenColor = GetColor( 0 , 120 , 0 );				// 绿色
	RedColor = GetColor( 200 , 60 , 60 );				// 红色

	DEPTH = HIGH;										// 搜索深度，先定为高级
	HumanMark = BlackColor;								// 初始为高级，变化色同棋色
	MarkPut = ComputerMark = WhiteColor;				// 初始为高级，变化色同棋色

	new_board( );										// 先初始一个经典 2 x 2 棋盘	
	put_value( );										// 给棋格位置赋权重

	SetMouseDispFlag( TRUE ) ;							// 设置鼠标为可见
	DrawBackground( );
	DrawBoard( );										// 棋盘 
	start(  ) ;											// 系统总控，等待读入菜单选项 	
	DxLib_End( ) ;										// DxLib库退出处理
	return 0 ;											// 退出程序
}

void start(  )											// 程序的主要控制函数
{
	int j, i  ;											// 列号、行号
	int click_menu ;                                
	int click, choice_menu;
	int fileOP = false;

	PrintMenu_1( );	 									// 打印一级菜单
	while(1) {	
		while( ProcessMessage( ) == 0 )
		{
			click = checkMouse( i , j, MENU1 ) ;
			click_menu = Menu_NONE;
			if(click)
			{
				GetMousePoint( &i , &j );
				click_menu = ClickRange( i , j, MENU1 );
			}

			if ( CheckHitKey( KEY_INPUT_ESCAPE ) != 0 )
				click_menu = Menu1_ESC ;
			else if ( CheckHitKey( KEY_INPUT_1 ) != 0 )
				click_menu = Menu1_New; 
			else if ( CheckHitKey( KEY_INPUT_2 ) != 0 )
				click_menu = Menu1_Read;
			else if ( CheckHitKey( KEY_INPUT_3 ) != 0 ) 
				click_menu = Menu1_Save; 

			DrawBox(oldbox.x1 , oldbox.y1 , oldbox.x2 , oldbox.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;	// 消影
			PrintMenu_1( );

			switch (click_menu) {
				case Menu1_New :							// 新棋盘
									new_board( );
									DrawBackground( );
									DrawBoard( );
									DrawText_2( "重新开始游戏，请阁下点鼠标或B/W键选择颜色 ", WhiteColor); 
									choose_Color( );
									play( );
									break;
				case Menu1_Read:							// 读棋盘
									fileOP = ReadBoard( ); 
									if ( Is_initialboard( ) )
										continue;			// 如果是原始2 X 2棋盘，则继续让玩家重新选择
									play( );				// 如果成功读入则用新文件；否则就是有老棋盘，接着下棋
									break ;
				case Menu1_Save:							// 存棋盘
									fileOP = SaveBoard( );
									if ( Is_initialboard( ) )
										continue;			// 如果是原始2 X 2棋盘，则继续让玩家重新选择
									DrawText_2( "游戏继续，请阁下点鼠标选择落子         ", WhiteColor); 
									play( );
									break;
				case Menu1_ESC:								// Escape 退出游戏
									while (1)
									{
										strcpy(ChoiceMenuStr, "退出选择(Y/N)：Y.退出游戏   N.不退出(继续)");
										DrawText_1(ChoiceMenuStr, WhiteColor );

										click = getchoice(choice_menu, CHOICE_YN, ChoiceMenuStr) ;
										if (!click || 
											((choice_menu != CHOICE1_YBL) && (choice_menu != CHOICE2_NWH))) 
											continue;

										if ( choice_menu == CHOICE1_YBL ) 
											return; 
										else
										{
											DrawText_2( "阁下选择不退出，请阁下继续游戏     ", WhiteColor);	
											break;
										}
									}
									break;

				default: break;
			}
		}
		return ;
	}
}

void play( )
{
	int m_h,m_c;
	int steps = 0;
	char now;
	int i,j;
	char StrBuf[ 128 ] , StrBuf2[ 32 ];
	int click_menu = Menu_NONE , click = false, choice_menu = CHOICE1_YBL;	// 赋初值，以免后面出错

	PrintMenu_2( );										// 打印二级菜单
	while(1)
	{
		possible();
		m_h = mobility(HUMAN);
		m_c = mobility(COMPUTER);
		if ((m_h==0) && (m_c==0))
			break;
		if (steps%2 == 0)
		{
			now=HUMAN;
		}
		else
		{
			DrawText_2( "稍安勿躁，容在下思考          ", WhiteColor ); 			
			now=COMPUTER;
		}

		if ( mobility(now) == 0 )
		{
			if (steps%2 == 0) 
				lstrcpy(  ChoiceMenuStr  , "玩家无棋可下，必须让棋！请阁下按任意键继续" ) ;
			else lstrcpy(  ChoiceMenuStr  , "电脑无棋可下，必须让棋！请阁下按任意键继续" );
			DrawText_1( ChoiceMenuStr, WhiteColor );
			WaitTimer(WaitSomeTime);					// 这里如果不wait, 就会一闪而过
			while( CheckHitKeyAll() == 0 )				// 等待用户输入任意键
			{
			}
	
			steps++;
			continue;
		}

		if (now == HUMAN)								// 该人下了
		{
			if (steps != 0)
			{
				lstrcpy( StrBuf , "轮到阁下了，方才在下落子 " ) ;	// StrBuf 中准备文字
				Coordinate(comp_x, comp_y, StrBuf2, 1 );	// 把坐标存入字符串
				lstrcat( StrBuf , StrBuf2 ) ; 			// StrBuf 与StrBuf2 拼接
				DrawText_2( StrBuf, WhiteColor ); 
			}

			//尚未有消息
			int out;
			while( ProcessMessage() == 0 )
			{
				out = false;
				click = checkMouse( i , j, MENU2 ) ;			// 读取鼠标按键位置
				click_menu = Menu_NONE;
				if(click)
				if((i < 0)||(i >= SCALE)||(j < 0)||(j >= SCALE))
				{
					GetMousePoint( &i, &j );
					click_menu = ClickRange( i , j, MENU2 );
					out = true;
				}

				if ( CheckHitKey( KEY_INPUT_ESCAPE ) != 0 )
					click_menu = Menu2_ESC ;
				else if ( CheckHitKey( KEY_INPUT_1 ) != 0 )
					click_menu = Menu2_Help; 
				else if ( CheckHitKey( KEY_INPUT_2 ) != 0 )
					click_menu = Menu2_Level;
				else if ( CheckHitKey( KEY_INPUT_3 ) != 0 ) 
					click_menu = Menu2_Save; 

				DrawBox(oldbox.x1 , oldbox.y1 , oldbox.x2 , oldbox.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;	// 消影
				// PrintMenu_2( );							这里曾经为了避免下划线覆盖而重新画菜单

				switch (click_menu) {
					case Menu2_Help : 
										help( );		// 报告玩家可落子的位置
										continue;
					case Menu2_Level: 
										Level( );
										continue;
					case Menu2_Save : 
										SaveBoard( );
										continue;
					case Menu2_ESC : 
										// Escape 退出游戏
										lstrcpy(ChoiceMenuStr, "退出选择(Y/N)：Y.退回上层   N.留本层(继续)");
										DrawText_1(ChoiceMenuStr , WhiteColor );
										WaitTimer(WaitSomeTime);	// 这里如果不wait, 将一闪而过
										click = getchoice(choice_menu, CHOICE_YN, ChoiceMenuStr ) ;
										if (!click || ((choice_menu != CHOICE1_YBL) && (choice_menu != CHOICE2_NWH)))
											continue;

										if ( choice_menu == CHOICE1_YBL )
											return; 
										else 
											DrawText_2( "阁下选择不退出，请继续游戏     ", WhiteColor);	
										break;
					default : 	
										break;
				}

				if(!out )
				if (( ( i >= 0 ) && ( i < SCALE ) && ( j >= 0 ) && ( j < SCALE ) )||(click_menu == 4))
				{
					if (!human[i][j]) 
						continue;

					if (board[i][j] != EMPTY)			// 人可以落子
						DrawChess( i, j );
					change_color(i,j,now,board,true);
					DrawBoard();
					steps++;
					break;
				}
			}
		}

		else
		{
			DrawText_2( "稍安勿躁，请容在下思考          ", WhiteColor ); 	
			find_position(0,board,-inf,inf);
			steps++;
			change_color(comp_x, comp_y, now,board, true);  

			DrawBoard( );
			ColorChess( comp_x, comp_y, MarkPut );
		}
	}
	PrintScore( );
	PrintMenu_1( );										// 打印一级菜单
}

void PrintMenu_1( )										// 打印一级菜单
{
	DrawText_3( "一级菜单：1(新盘) 2(读盘) 3(写盘) ESC(退出)", WhiteColor); 		
}

void PrintMenu_2( )										// 打印二级菜单
{
	DrawText_3( "二级菜单：1(求助) 2(棋力) 3(写盘) ESC(退回)", WhiteColor); 		
}

// "一级菜单：1(新盘) 2(读盘) 3(写盘) ESC(退出)"								
// "二级菜单：1(求助) 2(棋力) 3(写盘) ESC(退回)"
int ClickRange(int x,int y, int menuLevel)				// 菜单点击选项
{
	int range ;
	int y1 = 432, y2 = 454 , x1 = 0, x2 = 85;

	if((y > 452)||(y < 432)) 
		 range = Menu_NONE;								// 无效点击区域
	else if((x >= 93)&&(x <=160))
	{
		range = 1 ;										// 1区菜单
		x1 = 93;	x2 = 160;
	}
	else if((x >= 172)&&(x <= 238))
	{
		range = 2 ;										// 2区菜单
		x1 = 172;	x2 = 238;
	}
	else if((x >= 249)&&(x <= 316))
	{
		range = 3 ;										// 3区菜单
		x1 = 249;	x2 = 316;
	}
	else if((x >= 326)&&(x <= 416))
	{
		range = 0 ;										// ESC区菜单
		x1 = 326;	x2 = 416;
	}
	else range = Menu_NONE ;

	DrawBox(oldbox.x1 , oldbox.y1 , oldbox.x2 , oldbox.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;
	DrawBox(x1 - 2 , y2 , x2 + 2, y2 + 4 ,  GetColor( 0 , 200 , 0 ) , TRUE ) ;
//  这曾经是为了测试下划线是否影响了菜单选项内容的。现在不需要了
//	if (menuLevel == MENU1)
//		PrintMenu_1( );
//	else PrintMenu_2( );
	oldbox.x1 = x1 - 2; oldbox.y1 = y2; 
	oldbox.x2 = x2 + 2; oldbox.y2 = y2 + 4;

	return range ;
}

int ClickChoice(int x , int y, char str[] )
{
	int range ;
	int y1 = 357, y2 = 384 , x1 = 0, x2 = 130;

	DrawText_1(  ChoiceMenuStr , WhiteColor );

	if((y > 382)||(y < 357))							// 无效点击区域
		range = Menu_NONE;					
	else if((x >= 145) && (x <= 250))
	{
		range = CHOICE1_YBL;
		x1 = 145;	x2 = 250 ;							// 1区选项
	}
	else if((x > 275) && (x <= 400))
	{
		range = CHOICE2_NWH ;							// 2区选项
		x1 = 275;	x2 = 400 ;
	}
	else range = Menu_NONE ;

	DrawBox(oldboxYN.x1 , oldboxYN.y1 , oldboxYN.x2 , oldboxYN.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;
	DrawBox(x1 - 2 , y2, x2 + 2, y2 + 4 ,  GetColor( 0 , 200 , 0 ) , TRUE ) ;
	// DrawText_1( ChoiceMenuStr , WhiteColor );		这曾经是为了测试下划线是否影响了菜单选项内容的。现在不需要了

	oldboxYN.x1 = x1 - 2; oldboxYN.y1 = y2; 
	oldboxYN.x2 = x2 + 2; oldboxYN.y2 = y2 + 4;

	return range ;
}

void Level( )
{
	int choice_menu;
	int click;

	lstrcpy( ChoiceMenuStr , "棋力选择(L/H)：L.初级(翻转) H.高级(搜四层)");
	DrawText_1( ChoiceMenuStr, WhiteColor );

	while (1)
	{
		click = getchoice(choice_menu, CHOICE_LH, ChoiceMenuStr) ;
		if (!click || ((choice_menu != CHOICE1_YBL) && (choice_menu != CHOICE2_NWH)))
			continue;

		if( choice_menu == CHOICE1_YBL ) 
		{
			DrawText_2(  "低级棋力，将显示颜色翻转（玩家蓝，计算机红）", WhiteColor); 
			DEPTH = LOW;
			MarkTime = WaitSomeTime;						// 低级棋，给出足够时间，允许观察变化
			HumanMark = GetColor( 0 , 120 , 120 );			// 标记浅蓝色，是人刚变化的
			ComputerMark = GetColor( 100 , 20 , 20 );		// 标记浅红色，是计算机刚变化的
			MarkPut = RedColor = GetColor( 200 , 20 , 20 );	// 用于标记计算机刚下的颜色，红色
			break;
		}
		else 
		{
			DrawText_2(  "高级棋力，纯黑白游戏不显示颜色翻转              ", WhiteColor); 
			DEPTH = HIGH;
			MarkTime = 0;
			if ( HUMAN == BLACK) 
			{
				HumanMark = BlackColor;						
				MarkPut = ComputerMark = WhiteColor;					
			}
			else  
			{
				HumanMark = WhiteColor;						
				MarkPut = ComputerMark = BlackColor;					
			}
			break;
		}
	}
}

void help( )											// 查询可落子的棋盘格
{
	int i, j, count = 0;								// 计数，算出5个就够了
	char StrBuf[128], StrBuf2[28];
	lstrcpy( StrBuf , "可落子处" ) ;					// StrBuf 中存储菜单

	possible();
	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
			if(human[i][j])								// 人能落子
			{
				Coordinate( i, j, StrBuf2 ,1);
				lstrcat( StrBuf , StrBuf2 ) ; 			// StrBuf 与StrBuf2 拼接
				DrawText_2( StrBuf, RedColor ); 		
				
				count++;
				if (count >= 5)
					return;
			}
	if (count == 0)
	{
		lstrcpy( StrBuf , "玩家无处可落子        " ) ;	// StrBuf 中存储菜单
		DrawText_2( StrBuf, RedColor ); 		
	}
}

void new_board()
{
	int i,j;
    for(i=0;i<SCALE;i++)
        for(j=0;j<SCALE;j++)
        {
            board[i][j] = EMPTY;
        }
	// 初始棋局
	board[SCALE/2-1][SCALE/2-1] = WHITE;
	board[SCALE/2][SCALE/2] = WHITE;
	board[SCALE/2-1][SCALE/2] = BLACK;
	board[SCALE/2][SCALE/2-1] = BLACK;
	return;
/*  相当于
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, W, B, 0, 0, 0 } ,
	{ 0, 0, 0, B, W, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ; */
}

void DrawBackground( )									// 画棋盘背景 
{
	int i, j;											// 列、行坐标
	char StrBuf[128], ch[2];

	lstrcpy( StrBuf , "     A   B  C   D  E   F  G   H  " );
	DrawString( 0 , 0, StrBuf ,  WhiteColor ) ;
	DrawString( 0, (SCALE+1) * BOARD_SIZE + 9, StrBuf, WhiteColor); 
		
	// 画旁边的索引 0 - 7
	for( i = 1 ; i <= SCALE ; i ++ ) {
		ch[0] = (i-1) + '0';  ch[1] = '\0';
		lstrcpy( StrBuf , ch);
		lstrcat( StrBuf , "                                 " );
		lstrcat( StrBuf , ch);
		DrawString( 5, i * BOARD_SIZE + 9 , StrBuf, WhiteColor); 
	}
	
	// 画棋盘底色
	DrawBox( BOARD_SIZE , BOARD_SIZE - delta ,(SCALE+1) * BOARD_SIZE ,  (SCALE+1) * BOARD_SIZE - delta, 
				GreenColor , TRUE ); 
	// 画横线
	for( i = 2 ; i <= SCALE ; i ++ )
		DrawLine(BOARD_SIZE , i * BOARD_SIZE - delta, (SCALE+1) * BOARD_SIZE,  i * BOARD_SIZE - delta,  GetColor( 20 , 20 , 20) ) ;
	// 画竖线
	for( j = 2 ; j <= SCALE ; j ++ )
		DrawLine(j * BOARD_SIZE , BOARD_SIZE - delta,  j * BOARD_SIZE , (SCALE+1) * BOARD_SIZE - delta,  GetColor( 20 , 20 , 20) ) ;

	/*
	// 画棋盘底色
	DrawBox( BOARD_SIZE , BOARD_SIZE ,(SCALE+1) * BOARD_SIZE ,  (SCALE+1) * BOARD_SIZE , 
				GreenColor , TRUE ); 
	// 画横线
	for( i = 2 ; i <= SCALE ; i ++ )
		DrawLine(BOARD_SIZE , i * BOARD_SIZE , (SCALE+1) * BOARD_SIZE,  i * BOARD_SIZE ,  GetColor( 20 , 20 , 20) ) ;
	// 画竖线
	for( j = 2 ; j <= SCALE ; j ++ )
		DrawLine(j * BOARD_SIZE , BOARD_SIZE,  j * BOARD_SIZE , (SCALE+1) * BOARD_SIZE,  GetColor( 20 , 20 , 20) ) ;
	*/
}

void DrawBoard( )										// 画棋盘 
{
	int i, j;											// 列、行坐标
	for( i = 0 ; i < SCALE ; i ++ )
		for( j = 0 ; j < SCALE ; j ++ )
			if (board[ i ][ j ] != EMPTY ) 
				DrawChess( i, j );
}

void DrawChess( int i , int j ) 
{
	int size, chesscolor ;
	switch(board[i][j]) {
		case EMPTY:		break;							// 空格子
		case BLACK:		chesscolor = BlackColor;		// 黑棋 GetColor( 0 , 0 , 0 );					
						break;
		case WHITE:	    chesscolor = WhiteColor;		// 白棋 GetColor( 255 , 255 , 255 ) ;					
						break;
		default:		break;
	}
	size = int( BOARD_SIZE / CIRCLE_Adj );		

	DrawCircle( int(( i  + 1.5 ) * BOARD_SIZE ) , int(( j + 1.5 ) * BOARD_SIZE - delta ), 
			size, chesscolor, TRUE ) ;	
}

void MarkChess( int i , int j, char role ) 
{
	int size;
	size = int (BOARD_SIZE / CIRCLE_Adj);						// CIRCLE_Adj = 3.75 
	if (role == COMPUTER)
		DrawCircle( int(( i  + 1.5 ) * BOARD_SIZE ) , int(( j + 1.5 ) * BOARD_SIZE - delta ), 
			size, ComputerMark, TRUE ) ;	
	else DrawCircle( int(( i  + 1.5 ) * BOARD_SIZE ) , int(( j + 1.5 ) * BOARD_SIZE - delta), 
			size, HumanMark, TRUE ) ;
	WaitTimer(MarkTime);
}

void ColorChess( int i , int j, int chesscolor ) 
{
	int size;
	size = int (BOARD_SIZE / CIRCLE_Adj );						// CIRCLE_Adj = 3.75 
	DrawCircle( int(( i  + 1.5 ) * BOARD_SIZE ) , int(( j + 1.5 ) * BOARD_SIZE  - delta ), 
			size, chesscolor, TRUE ) ;	
}

void DrawText_1( char StrBuf[], int color )				// 清空第一行提示区域，再打印新提示
{
	DrawBox( 0 , (SCALE + 2) * BOARD_SIZE, 639 ,  (SCALE + 2) * BOARD_SIZE + 24 , BlackColor , TRUE ) ;
	DrawString( 0 , (SCALE + 2) * BOARD_SIZE + 2, StrBuf , color ) ;
}

void DrawText_2( char StrBuf[], int color )				// 清空第二行提示区域，再打印新提示
{
	DrawBox(0 , ( SCALE + 3 ) * BOARD_SIZE, 639 ,  ( SCALE + 3 )  * BOARD_SIZE + 20 , BlackColor , TRUE ) ;
	DrawString( 0 , ( SCALE + 3 ) * BOARD_SIZE + 2, StrBuf , color ) ;
}

void DrawText_3( char StrBuf[], int color )				// 清空第三行提示区域，再打印新提示
{
	DrawBox(0 , ( SCALE + 4 ) * BOARD_SIZE, 639 ,  ( SCALE + 4 )  * BOARD_SIZE + 20 , BlackColor , TRUE ) ;
	DrawString( 0 , ( SCALE + 4 ) * BOARD_SIZE + 2, StrBuf , color ) ;
}

void Coordinate(int i, int j, char StrBuf[ ], int flag)
{
	char StrBuf2[ 32 ], ch[5];
	if ((i >= 0 && i <= 7 )&&(flag))
	{
		ch[0] = i + 'A';		 
	    ch[1] = '\0';
	}
	else
		_itoa( i , ch , 10 );							
	lstrcpy( StrBuf , "(" ) ;
	lstrcat( StrBuf , ch ) ;							// StrBuf 与ch 拼接
	lstrcat( StrBuf , ", " ) ;							// StrBuf 与ch 拼接
	_itoa( j , StrBuf2 , 10 ) ;							// MouseY 值转成竖坐标格文字传入StrBuf2
	lstrcat( StrBuf , StrBuf2 ) ; 						// StrBuf 与StrBuf2 拼接
	lstrcat( StrBuf , ") " ) ;
}

int  checkMouse( int &i , int &j , int menuLevel )
{
	char StrBuf[ 128 ] , StrBuf2[ 32 ] ;
	int MouseInput ;
	int MouseX , MouseY ;

	// 测试鼠标
	while( CheckHitKeyAll() == 0 )
	{
		GetMousePoint( &MouseX , &MouseY ) ;			// 获取鼠标位置

		i = int ( MouseX / BOARD_SIZE ) - 1;	
		j = int (( MouseY + delta ) / BOARD_SIZE ) - 1;

		lstrcpy( StrBuf , "鼠标位置：" ) ;
		if( i<8 && j<8 && i>=0 && j>=0 )
		    Coordinate( i, j, StrBuf2 ,1);
		else
		{
			Coordinate( MouseX, MouseY, StrBuf2 , 0);
		}
		lstrcat( StrBuf , StrBuf2 ) ; 					// StrBuf 与StrBuf2 拼接

		MouseInput = GetMouseInput() ;
		DrawText_1(StrBuf, WhiteColor); 
		
		// 判断是否鼠标左键确认输入
		if ( ( MouseInput && MOUSE_INPUT_LEFT ) == 0 )
		{
			if( ProcessMessage() == -1 )				// 事件处理,即鼠标点击
				break ;									// 鼠标输入
		}

		ClickRange(MouseX, MouseY, menuLevel);
		
		WaitTimer( 100 ) ;								// 等待一定时间，以免屏幕上鼠标值变化太快
	}

	if(( MouseInput && MOUSE_INPUT_LEFT ) == 0)			// 鼠标点击
		return true;
	return false ;										// 键盘输入
}

void PrintScore(  )
{
	char StrBuf[ 128 ] , StrBuf2[ 32 ];
	int countHuman = 0, countComputer =0; 

	int i, j;

	for( i = 0 ; i < SCALE ; i ++ )
	{
		for( j = 0 ; j < SCALE ; j ++ )
		{
			if ( board[ i ][ j ] == HUMAN ) 
				countHuman ++ ;
			else if ( board[ i ][ j ] == COMPUTER )
				countComputer ++ ;
		}
	}

	lstrcpy( StrBuf , "玩家：" ) ;						// StrBuf 中准备"玩家得分" 的文字
	_itoa( countHuman , StrBuf2 , 10 ) ;				// 玩家得分值转成横坐标格文字传入StrBuf2
	lstrcat( StrBuf , StrBuf2 ) ;						// StrBuf 与StrBuf2 拼接
	lstrcat( StrBuf , "  计算机：" ) ;					// StrBuf 内容增加"计算机得分"
	_itoa( countComputer , StrBuf2 , 10 ) ;				// 计算机得分值转成竖坐标格文字传入StrBuf2
	lstrcat( StrBuf , StrBuf2 ) ; 						// StrBuf 与StrBuf2 拼接

	if ( countHuman < countComputer)
		lstrcat( StrBuf , " 很遗憾~阁下输了~！");
	else if(countHuman > countComputer )
		lstrcat( StrBuf ,"  天哪，给大神跪了！\n");
	else
		lstrcat( StrBuf , " 平手啊~~~难分胜负");
	DrawText_2( StrBuf, RedColor); 			
}

int getchoice(int &click_menu, int type, char str[ ])		
{
	int i, j;
	int click = true, Choice1, Choice2 ;
	int MouseInput = -1;
	int MouseX , MouseY ;

	switch(type) {
		case CHOICE_YN:
						Choice1 = KEY_INPUT_Y;
						Choice2 = KEY_INPUT_N;
						break;
		case CHOICE_BW: 
						Choice1 = KEY_INPUT_B;
						Choice2 = KEY_INPUT_W;
						break;
		case CHOICE_LH:
						Choice1 = KEY_INPUT_L;
						Choice2 = KEY_INPUT_H;
						break;
		default:
						break;
	}

	click_menu = Menu_NONE;
	// 测试鼠标
	while( CheckHitKeyAll() == 0 )
	{
		GetMousePoint( &MouseX , &MouseY ) ;			// 获取鼠标位置
		MouseInput = GetMouseInput() ;

		// 判断是否鼠标左键确认输入
		if ( ( MouseInput && MOUSE_INPUT_LEFT ) == 0 )
		{
			if( ProcessMessage() == -1 ) {				// 事件处理,即鼠标点击
				break ;									// 鼠标输入
			}
		}
		ClickChoice(MouseX, MouseY,  ChoiceMenuStr );
		WaitTimer( 100 ) ;								// 等待一定时间，以免屏幕上鼠标值变化太快
	}

	// 键盘输入
	if ( CheckHitKey( Choice1 ) != 0 )					// YBL键
		click_menu = CHOICE1_YBL;
	else if (( CheckHitKey( Choice2 ) != 0 ) )			// NWH键
		click_menu = CHOICE2_NWH;
	else if(( MouseInput && MOUSE_INPUT_LEFT ) == 0) 	// 鼠标点击
	{
		GetMousePoint( &i , &j );
		click_menu = ClickChoice( i , j, str );
	}
	else click = false;									// 选择了其他键，则为假

	DrawBox(oldboxYN.x1 , oldboxYN.y1 , oldboxYN.x2 , oldboxYN.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;	// 消影
	DrawText_1(  ChoiceMenuStr , WhiteColor );
	return click;
}

void choose_Color( )
{
	int choice_menu;
	int click;

	lstrcpy(ChoiceMenuStr, "棋色选择(B/W)：B.黑子(先手) W.白子(后手下)");
	DrawText_1( ChoiceMenuStr, WhiteColor );
	
	while( !ProcessMessage() )
	{
		click = getchoice(choice_menu, CHOICE_BW, ChoiceMenuStr) ;
		if (!click || ((choice_menu != CHOICE1_YBL) && (choice_menu != CHOICE2_NWH)))
			continue;

		if( choice_menu == CHOICE1_YBL )
		{
			HUMAN=BLACK;
			MarkPut=COMPUTER=WHITE;
			HumanMark = BlackColor;						
			MarkPut = ComputerMark = WhiteColor;
			
			possible( );
			DrawText_2( "黑子先手，阁下先请                  ", WhiteColor); 
			DrawBoard();
			break;									
		}
		else if ( choice_menu == CHOICE2_NWH ) 
		{
			HUMAN=WHITE;
			MarkPut=COMPUTER=BLACK;						// 忘记修改markput可是很恐怖的
			HumanMark = WhiteColor;						
			MarkPut = ComputerMark = BlackColor;
			possible( );
			DrawText_2(  "白子后手，承蒙阁下谦让，在下不客气了", WhiteColor); 

			srand((unsigned)time(NULL));				// 初始化
			int a=rand();								// 随机数
			int can[60][2];
			int num=0;
			int i,j;
			for(i=0;i<SCALE;i++)
			{
				for(j=0;j<SCALE;j++)
				{
					if(computer[i][j])
					{
						can[num][0]=i;
						can[num][1]=j;
						num++;
					}
				}
			}
			change_color(can[a%num][0],can[a%num][1],COMPUTER,board, true);
			ColorChess( can[a%num][0],can[a%num][1], MarkPut );   
			DrawBoard( );    //debug
			possible( );
			break;
		}
	}
}

int SaveBoard()										// 向文件中写棋盘数据
{
	int i, j, k = 0;                          
	int click;
	int choice_menu; 
	char StrBuf[ 128 ] ;				// 提示信息用的缓冲

	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;  // 呵呵……这么长的程序都编过来了，最后一个错是设错了数组长度以至于栈溢出……

	ch = '0';
	DrawText_1(  "写盘。请阁下输入文件名（<30）： " , WhiteColor );
	DrawText_2( "                                             ", WhiteColor); 

	KeyInputString( 0 , ( SCALE + 3 ) * BOARD_SIZE, 639 , filename , FALSE ); // gets(filename);
	if (_access(filename, 0) == 0)   // 已经存在
	{
		lstrcpy( StrBuf , filename ) ;	
		lstrcat( StrBuf , "文件已经存在                 " ) ;	
		DrawText_2( StrBuf, WhiteColor);
		lstrcpy( ChoiceMenuStr, "覆盖选择(Y/N)： Y.覆盖文件  N.不覆盖文件" );
		DrawText_1( ChoiceMenuStr, WhiteColor);	

		while( !ProcessMessage() )
		{
			click = getchoice(choice_menu, CHOICE_YN, ChoiceMenuStr ) ;
			if (!click || ((choice_menu != CHOICE1_YBL) && (choice_menu != CHOICE2_NWH)))
				continue;

			if ( choice_menu == CHOICE2_NWH )
			{											// 选择N
				DrawText_2( "阁下选择不覆盖文件，请继续游戏盘  ", WhiteColor );
				return false;
			}
			else break ;
		}
	}

	err  = fopen_s( &fp, filename, "w+" );

	if( err != 0 )
	{
		lstrcpy( StrBuf , filename ) ;	
		lstrcat( StrBuf , "文件打开失败              " );
		DrawText_2( StrBuf , WhiteColor ); 
		return false;
	}
	else {
		lstrcpy( StrBuf , filename ) ;
		lstrcat( StrBuf , "文件成功保存，请阁下继续游戏   " );
		DrawText_2( StrBuf, WhiteColor ); 
	}

	for(j=0;j<SCALE;j++)								// 存盘
	{
		for(i=0;i<SCALE;i++) 
			str[k++] = board[i][j];
		str[k++]='\n';									// 加回车
	}
	str[k++]=COMPUTER; 
	str[k++]=HUMAN;

    fwrite(str,1,(SCALE+1)*SCALE+2,fp);	// 函数原型size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
										// 注意，这里 str并没有保存"\0"字符串结束符，不能用    fwrite(str,1,strlen(str),fp);	
    fclose(fp);											// 记得关上……
	return true;
}

//判断是否合法棋盘的系列函数

//递归专用
void walk(int i, int j, int b[SCALE][SCALE])
{
	if((i<0)||(i>=SCALE)||(j<0)||(j>=SCALE))
		return;											// 出界返回
	if(!b[i][j])										// 空值返回
		return;
	b[i][j]=0;
	walk(i-1,j-1,b);									// 向八方蔓延
	walk(i-1,j,b);
	walk(i-1,j+1,b);
	walk(i,j-1,b);
	walk(i,j+1,b);
	walk(i+1,j-1,b);
	walk(i+1,j,b);
	walk(i+1,j+1,b);
}

//判断棋盘
int board_check(char a[SCALE][SCALE])
{
	int temp[SCALE][SCALE];
	int i,j;

	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if((a[i][j]==BLACK)||(a[i][j]==WHITE))
				temp[i][j]=1;
			else
				if(a[i][j]==EMPTY)
				    temp[i][j]=0;
				else								// 存在非法字符
					return IllegalChar;
		}
	}
	for(i=SCALE/2-1;i<=SCALE/2;i++)
	{
		for(j=SCALE/2-1;j<=SCALE/2;j++)
			if(!temp[i][j])							// 如果中心没有棋子，一定非法
				return NoCentralChess;
	}
	walk(SCALE/2,SCALE/2,temp);
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
			if(temp[i][j])
				return NotConnected;				// 未连成一片，一定不行
	}
	return GoodBoard;
}

bool ReadBoard()										// 从文件中读入棋盘数据
{
	int i, j, k = 0;
	char StrBuf[ 128 ] ;								// 提示信息用的缓冲

	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;  // 呵呵……这么长的程序都编过来了，最后一个错是设错了数组长度以至于栈溢出……
	int  board_status;
	 
	ch='0';

	DrawText_1( "读盘。请阁下输入文件名（<30）： ", WhiteColor );
	DrawText_2( "                                              ", WhiteColor); 

	KeyInputString( 0 , ( SCALE + 3) * BOARD_SIZE, 639 , filename , FALSE ); // gets(filename);
	err  = fopen_s( &fp, filename, "r" );
	if( err != 0 ) {
		lstrcpy( StrBuf , filename ) ;	
		lstrcat( StrBuf , " 文件打开失败。请按任意键继续" );
		DrawText_2( StrBuf, WhiteColor ); 
		WaitTimer(WaitSomeTime);						// 这里如果不wait, 就会一闪而过
		while( CheckHitKeyAll() == 0 )					// 等待用户输入任意键
		{
		}
		return false;
	}

	fread(str,1,SCALE*(SCALE+1)+2,fp);	
	// 函数原型size_t fread(void *buffer, size_t size, size_t count, FILE *stream);　

	for (j=0;j<SCALE;j++)
	{
		for(i=0;i<SCALE;i++)  {
			save_board[i][j] = board[i][j]; 
			board[i][j] = str[k++];
		}
		k++;											// 去掉回车
	}
	char computer=str[k++];
	char human=str[k++];

    fclose(fp);											// 用过就关好习惯

	board_status = board_check( board );
										
	switch (board_status) {
		case IllegalChar :
							lstrcpy( StrBuf , filename ) ;
							lstrcat( StrBuf , " 存在无法识别的符号。请按任意键继续" );
							break;
		case NoCentralChess:
							lstrcpy( StrBuf , filename ) ;
							lstrcat( StrBuf , " 棋盘中没有中心棋子。请按任意键继续" );	
							break;
		case NotConnected :
							lstrcpy( StrBuf , filename ) ;
							lstrcat( StrBuf , " 棋盘中棋子不连续。请按任意键继续" );
							break;
		case GoodBoard :
							HUMAN = human;
							COMPUTER = computer;
							lstrcpy( StrBuf , filename ) ;
							lstrcat( StrBuf , " 棋盘文件成功读入，" ) ;
							if ( HUMAN == BLACK) 
								lstrcat(StrBuf, "玩家执黑棋") ;
							else lstrcat(StrBuf, "玩家执白棋") ;
		default :
							break;
	}

	DrawText_2( StrBuf, WhiteColor ); 
	if (board_status != IllegalChar) {
		DrawBackground( );	DrawBoard( );					// 画新读入的文件棋盘内容
	}
	WaitTimer(WaitSomeTime);								// 这里如果不wait, 就会一闪而过
	while( CheckHitKeyAll() == 0 )							// 等待用户输入任意键
	{
	}

	if (board_status != GoodBoard) 
	{
		for (j=0;j<SCALE;j++)
			for(i=0;i<SCALE;i++)  
				board[i][j] = save_board[i][j];
		lstrcpy( StrBuf , "文件读入失败，请阁下继续游戏" );
		DrawText_2( StrBuf, WhiteColor ); 
		DrawBackground( ); DrawBoard( );					// 复原
		return false ;
	}
	else return true ;
}

void put_value()
{
	int i,j;
	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
			const_value[i][j]=1;
	const_value[0][0]=const_value[0][SCALE-1]
	=const_value[SCALE-1][0]=const_value[SCALE-1][SCALE-1]=5000;
	//origin:800
	int edge = 300;
	//origin:100
	for(i=1;i<SCALE-1;i++)
	{
		const_value[0][i]=edge;
		const_value[SCALE-1][i]=edge;
		const_value[i][0]=edge;
		const_value[i][SCALE-1]=edge;
	}
	int corner = -1000;
	//origin:-200
	const_value[0][1]=const_value[1][0]=const_value[1][1]=corner;
	const_value[0][SCALE-2]=const_value[1][SCALE-2]=const_value[1][SCALE-1]=corner;
	const_value[SCALE-1][1]=const_value[SCALE-2][0]=const_value[SCALE-2][1]=corner;
	const_value[SCALE-1][SCALE-2]=const_value[SCALE-2][SCALE-1]=const_value[SCALE-2][SCALE-2]=corner;

	const_value[0][3]=const_value[0][4]=-100;
	const_value[SCALE-1][3]=const_value[SCALE-1][4]=-100;
	const_value[3][0]=const_value[4][0]=-10;
	const_value[3][SCALE-1]=const_value[4][SCALE-1]=-100;
}

//判断能否落子的函数
int judge_put(int i, int j, char whoseturn, char board[SCALE][SCALE])
{
    if(board[i][j]!=EMPTY)								// 不能重叠放子
		return 0;
	char against;
	if(whoseturn==COMPUTER)
		against=HUMAN;
	else
		against=COMPUTER;
	int k;
	if(board[i][j]!=EMPTY)								// 不能重叠放子
		return 0;
	//上
	if(i>0)
	{
		if(board[i-1][j]==against)
		{
			for(k=i-2;k>=0;k--)
			{
				if(board[k][j]==EMPTY)
					break;
				if(board[k][j]==whoseturn)
					return 1;
			}
		}
	}
	//下
	if(i<SCALE-1)
	{
		if(board[i+1][j]==against)
		{
			for(k=i+2;k<SCALE;k++)
			{
				if(board[k][j]==EMPTY)
					break;
				if(board[k][j]==whoseturn)
					return 1;
			}
		}
	}
	//左
	if(j>0)
	{
		if(board[i][j-1]==against)
		{
			for(k=j-2;k>=0;k--)
			{
				if(board[i][k]==EMPTY)
					break;
				if(board[i][k]==whoseturn)
					return 1;
			}
		}
	}
	//右
	if(j<SCALE-1)
	{
		if(board[i][j+1]==against)
		{
			for(k=j+2;k<SCALE;k++)
			{
				if(board[i][k]==EMPTY)
					break;
				if(board[i][k]==whoseturn)
					return 1;
			}
		}
	}
	//左上
	if((i>0)&&(j>0))
	{
		if(board[i-1][j-1]==against)
		{
			for(k=2;((i-k>=0)&&(j-k>=0));k++)
			{
				if(board[i-k][j-k]==EMPTY)
					break;
				if(board[i-k][j-k]==whoseturn)
					return 1;
			}
		}
	}
	//右上
	if((i>0)&&(j<SCALE-1))
	{
		if(board[i-1][j+1]==against)
		{
			for(k=2;((i-k>=0)&&(j+k<SCALE));k++)
			{
				if(board[i-k][j+k]==EMPTY)
					break;
				if(board[i-k][j+k]==whoseturn)
					return 1;
			}
		}
	}
	//左下
	if((i<SCALE-1)&&(j>0))
	{
		if(board[i+1][j-1]==against)
		{
			for(k=2;(i+k<SCALE)&&(j-k>=0);k++)
			{
				if(board[i+k][j-k]==EMPTY)
					break;
				if(board[i+k][j-k]==whoseturn)
					return 1;
			}
		}
	}
	//右下
	if((i<SCALE-1)&&(j<SCALE-1))
	{
		if(board[i+1][j+1]==against)
		{
			for(k=2;((i+k<SCALE)&&(j+k<SCALE));k++)
			{
				if(board[i+k][j+k]==EMPTY)
					break;
				if(board[i+k][j+k]==whoseturn)
					return 1;
			}
		}
	}
	return 0;
}

//更新能否落子信息的函数
void possible()
{
	int i, j;
	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
		{
			human[i][j]=computer[i][j]=0;
			if(judge_put(i,j,HUMAN,board))
				human[i][j]=1;
			if(judge_put(i,j,COMPUTER,board))
				computer[i][j]=1;
		}
}

//判断行动力的函数
int mobility(char this_side)
{
	int i,j;
	int n=0;
	if(this_side==COMPUTER)
	{
	    for(i=0;i<SCALE;i++)
		    for(j=0;j<SCALE;j++)
			    if(computer[i][j])
					n++;
	}
	else  // bug= =b没加大括号（我就是懒了一下，至于吗……）
	{
		for(i=0;i<SCALE;i++)
			for(j=0;j<SCALE;j++)
				if(human[i][j])
					n++;
	}
	return n;
}

//落子之后改变棋盘的函数
void change_color(int i, int j, char whoseturn,char board[SCALE][SCALE], bool flag)
{
	int x,y;
	int k;
	char against;

	int m, n;

	for (m=0;m<SCALE;m++)
		for(n=0;n<SCALE;n++) 
			mark_board[m][n] = EMPTY;

	if (whoseturn == HUMAN)
		against = COMPUTER;
	else
		against = HUMAN;
	//上
	board[i][j] = whoseturn;
	if(i>0)
	{
		if(board[i-1][j]==against)
		{
			for(k=i-2;k>=0;k--)
			{
				if(board[k][j]==EMPTY)
					break;
				if(board[k][j]==whoseturn)
				{
					for(x=i-1;x>k;x--) {
						board[x][j]=whoseturn;
						mark_board[x][j] = MARK;
						if (flag)
							MarkChess( x, j, whoseturn);  
					}
					break;
				}
			}
		}
	}
	//下
	if(i<SCALE-1)
	{
		if(board[i+1][j]==against)
		{
			for(k=i+2;k<SCALE;k++)
			{
				if(board[k][j]==EMPTY)
					break;
				if(board[k][j]==whoseturn)
				{
					for(x=i+1;x<k;x++)	{
						board[x][j]=whoseturn;
						mark_board[x][j] = MARK;
						if (flag)
							MarkChess( x, j, whoseturn);  
					}
					break;
				}
			}
		}
	}
	//左
	if(j>0)
	{
		if(board[i][j-1]==against)
		{
			for(k=j-2;k>=0;k--)
			{
				if(board[i][k]==EMPTY)
					break;
				if(board[i][k]==whoseturn)
				{
					for(x=j-1;x>k;x--) {
						board[i][x]=whoseturn;
						mark_board[i][x] = MARK;
						if (flag)
							MarkChess( i, x, whoseturn);   
					}
					break;
				}
			}
		}
	}
	//右
	if(j<SCALE-1)
	{
		if(board[i][j+1]==against)
		{
			for(k=j+2;k<SCALE;k++)
			{
				if(board[i][k]==EMPTY)
					break;
				if(board[i][k]==whoseturn)
				{
					for(x=j+1;x<k;x++) {
						board[i][x]=whoseturn;
						mark_board[i][x] = MARK;
						if (flag)
							MarkChess( i, x, whoseturn);  
					}
					break;
				}
			}
		}
	}
	//左上
	if((i>0)&&(j>0))
	{
		if(board[i-1][j-1]==against)
		{
			for(k=2;((i-k>=0)&&(j-k>=0));k++)
			{
				if(board[i-k][j-k]==EMPTY)
					break;
				if(board[i-k][j-k]==whoseturn)
				{
					for(x=i-1,y=j-1;x>i-k;x--,y--) {
						board[x][y]=whoseturn;
						mark_board[x][y] = MARK;
						if (flag)
							MarkChess( x,y, whoseturn);  
					}
					break;
				}
			}
		}
	}
	//右上
	if((i>0)&&(j<SCALE-1))
	{
		if(board[i-1][j+1]==against)
		{
			for(k=2;((i-k>=0)&&(j+k<SCALE));k++)
			{
				if(board[i-k][j+k]==EMPTY)
					break;
				if(board[i-k][j+k]==whoseturn)
				{
					for(x=i-1,y=j+1;x>i-k;x--,y++) {
						board[x][y]=whoseturn;
						mark_board[x][y] = MARK;
						if (flag)
							MarkChess( x,y, whoseturn );  
					}
					break;
				}
			}
		}
	}
	//左下
	if((i<SCALE-1)&&(j>0))
	{
		if(board[i+1][j-1]==against)
		{
			for(k=2;(i+k<SCALE)&&(j-k>=0);k++)
			{
				if(board[i+k][j-k]==EMPTY)
					break;
				if(board[i+k][j-k]==whoseturn)
				{
					for(x=i+1,y=j-1;x<i+k;x++,y--) {
						board[x][y]=whoseturn;
						mark_board[x][y] = MARK;
						if (flag)
							MarkChess( x,y, whoseturn);  
					}
					break;
				}
			}
		}
	}
	//右下
	if((i<SCALE-1)&&(j<SCALE-1))
	{
		if(board[i+1][j+1]==against)
		{
			for(k=2;((i+k<SCALE)&&(j+k<SCALE));k++)
			{
				if(board[i+k][j+k]==EMPTY)
					break;
				if(board[i+k][j+k]==whoseturn)
				{
					for(x=i+1,y=j+1;x<i+k;x++,y++) {
						board[x][y]=whoseturn;
						mark_board[x][y] = MARK;
						if (flag)
							MarkChess( x,y, whoseturn);  
					}
					break;
				}
			}
		}
	}
}


//寻找最优点落棋的系列函数

//评定稳定系数的函数
int steady(int x, int y, char thisside, char board[SCALE][SCALE])
{
	char opposite;
	if(thisside==COMPUTER)
		opposite=HUMAN;
	else
		opposite=COMPUTER;
	int i,j;
	int k;
	int z=0;
	//横向
	for(j=y-1;j>=0;j--)
	{
		if(board[x][j]==EMPTY)
		{
			z--;
			for(k=y+1;k<SCALE;k++)
			{
				if(board[x][k]==EMPTY)
				{
					z++;
					break;
				}
				if(board[x][k]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
		if(board[x][j]==opposite)
		{
			z--;
			for(k=y+1;k<SCALE;k++)
			{
				if(board[x][k]==EMPTY)
				{
					z--;
					break;
				}
				if(board[x][k]==opposite)
				{
					z++;
					break;
				}
			}
			break;
		}
	}
	z++;
	//纵向
	for(i=x-1;i>=0;i--)
	{
		if(board[i][y]==EMPTY)
		{
			z--;
			for(k=x+1;k<SCALE;k++)
			{
				if(board[k][y]==EMPTY)
				{
					z++;
					break;
				}
				if(board[k][y]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
		if(board[i][y]==opposite)
		{
			z--;
			for(k=x+1;k<SCALE;k++)
			{
				if(board[k][y]==EMPTY)
				{
					z--;
					break;
				}
				if(board[k][y]==opposite)
				{
					z++;
					break;
				}
			}
			break;
		}
	}
	z++;
	//左上右下
	for(i=x-1,j=y-1;(i>=0)&&(j>=0);i--,j--)
	{
		if(board[i][j]==EMPTY)
		{
			z--;
			for(k=1;(x+k<SCALE)&&(y+k<SCALE);k++)
			{
				if(board[x+k][y+k]==EMPTY)
				{
					z++;
					break;
				}
				if(board[x+k][y+k]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
		if(board[i][j]==opposite)
		{
			z--;
			for(k=1;(x+k<SCALE)&&(y+k<SCALE);k++)
			{
				if(board[x+k][y+k]==EMPTY)
				{
					z--;
					break;
				}
				if(board[x+k][y+k]==opposite)
				{
					z++;
					break;
				}
			}
			break;
		}
	}
	z++;
	// 左下右上
	for(i=x-1,j=y+1;(i>=0)&&(j<SCALE);i--,j++)
	{
		if(board[i][j]==EMPTY)
		{
			z--;
			for(k=1;(x+k<SCALE)&&(y-k>=0);k++)
			{
				if(board[x+k][y-k]==EMPTY)
				{
					z++;
					break;
				}
				if(board[x+k][y-k]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
		if(board[i][j]==opposite)
		{
			z--;
			for(k=1;(x+k<SCALE)&&(y-k>=0);k++)
			{
				if(board[x+k][y-k]==EMPTY)
				{
					z++;
					break;
				}
				if(board[x+k][y-k]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
	}
	z++;
	return z;
}


//估值函数
//给当前棋局估值
//寻找最优点落棋的系列函数

void c_val(char board[SCALE][SCALE])   //计算可变权重
{
	int i,j;
	int n=SCALE-1;
	for(i=0;i<=n;i++)
		for(j=0;j<=n;j++)
			change_value[i][j]=0;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))	// 上
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))	// 下
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))	// 左
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))	// 右
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	// 对方占了角的情况（粗略处理）
	if(board[0][0]==HUMAN)
	    change_value[0][1]=change_value[1][0]=change_value[1][1] =- 300;
	if(board[0][SCALE-1]==HUMAN)
	    change_value[0][SCALE-2]=change_value[1][SCALE-2]=change_value[1][SCALE-1] =- 300;
	if(board[SCALE-1][0]==HUMAN)
	    change_value[SCALE-1][1]=change_value[SCALE-2][0]=change_value[SCALE-2][1] =- 300;
	if(board[SCALE-1][SCALE-1]==HUMAN)
	    change_value[SCALE-1][SCALE-2]=change_value[SCALE-2][SCALE-1]=change_value[SCALE-2][SCALE-2] =- 300;
	// 对方占边的策略变化粗处理
	for(i=1;i<SCALE-1;i++)
	{
		if(board[0][i]==HUMAN)
		{
			if((i != 1)&&(change_value[0][i-2] != HUMAN))
				change_value[0][i-1]-=300;
			if((i != SCALE-2)&&(change_value[0][i+2] != HUMAN))
				change_value[0][i+1]-=300;
		}
		if(board[SCALE-1][i]==HUMAN)
		{
			if((i != 1)&&(change_value[SCALE-1][i-2] != HUMAN))
				change_value[SCALE-1][i-1]-=300;
			if((i != SCALE-2)&&(change_value[SCALE-1][i+2] != HUMAN))
				change_value[SCALE-1][i+1]-=300;
		}
		if(board[i][0]==HUMAN)
		{
			if((i != 1)&&(change_value[i-2][0] != HUMAN))
				change_value[i-1][0]=-300;
			if((i != SCALE-2)&&(change_value[i+2][0] != HUMAN))
				change_value[i+1][0]=-300;
		}
		if(board[i][SCALE-1]==HUMAN)
		{
			if((i != 1)&&(change_value[i-2][SCALE-1] != HUMAN))
				change_value[i-1][SCALE-1]-=300;
			if((i != SCALE-2)&&(change_value[i+2][SCALE-1] != HUMAN))
				change_value[i+1][SCALE-1]-=300;
		}
	}
	//即将占角的情况粗略处理
	int fight = 1000;
	//origin:300
	//左上角
	if(board[0][1]!=EMPTY)
		change_value[0][0]+=fight;
	if(board[1][0]!=EMPTY)
		change_value[0][0]+=fight;
	if(board[1][1]!=EMPTY)
		change_value[0][0]+=fight;
	//右上角
	if(board[0][SCALE-2]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	if(board[1][SCALE-2]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	if(board[1][SCALE-1]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	//左下角
	if(board[SCALE-1][1]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	if(board[SCALE-2][1]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	if(board[SCALE-2][0]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	//右下角
	if(board[SCALE-2][SCALE-2]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
	if(board[SCALE-1][SCALE-2]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
	if(board[SCALE-2][SCALE-1]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
}


//估值函数
//给当前棋局估值

int temp_value(char board[SCALE][SCALE])		// 对低中高级有不同的计算权重方法
{
	for(int i = 0 ; i < SCALE ; i++)
		for(int j = 0 ; j < SCALE ; j++)
			change_value[i][j] = 0;
	if ( DEPTH == HIGH )						// 棋力是中低级就不计算动态权重（只计算高级）
		c_val(board);
	int score=0;
	int i,j;
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(board[i][j]==COMPUTER)  
			{
				if(DEPTH != LOW)		// 若低级棋力则在计算机下的时候不计算稳定性。让玩家开心
					score+=steady(i,j,COMPUTER,board)*30;	// 稳定性
				score+=(const_value[i][j]+change_value[i][j]);      //位置固有权重+可变权重
			}
			else
				if(board[i][j]==HUMAN)
				{
					score-=steady(i,j,HUMAN,board)*30;
					score-=(const_value[i][j]-change_value[i][j]);  //注意change_value全是以计算机角度为准的！
				}
			if(DEPTH != LOW)
			{
				if(judge_put(i,j,COMPUTER,board))			// 行动力
					score+=100;
				if(judge_put(i,j,HUMAN,board))
					score-=100;
			}
			else
			{
				int num_c = 1000; 
				int num_h = 1000;
				//origin:400//origin:500//origin:600
				if(judge_put(i,j,COMPUTER,board))			// 行动力
					score+=num_c;
					//score += (const_value[i][j] + change_value[i][j]);
				if(judge_put(i,j,HUMAN,board))
					score-=num_h;
					//score -= (const_value[i][j] + change_value[i][j]);
					//score -= const_value[i][j];
			}
		}
	}
	return score;
}

// 加alpha-beta剪枝
long long find_position(int depth, char last_board[SCALE][SCALE], long long alpha, long long beta)
{
	long long temp;										// 分数
	int temp_x=0;
	int temp_y=0;
	char now;
	if(depth%2==0)
	{
		now=COMPUTER;
	}
	else
	{
		now=HUMAN;
	}
	if(depth==DEPTH)
		return temp_value(last_board);
	char temp_board[SCALE][SCALE];
	int i,j;
	int p,q;
	int a=0;
	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
			temp_board[i][j]=last_board[i][j];
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(judge_put(i,j,now,last_board))			// 可以落子之处
			{
				change_color(i,j,now,temp_board,false);
				temp=find_position(depth+1, temp_board, alpha, beta);
				for(p=0;p<SCALE;p++)
					for(q=0;q<SCALE;q++) 
					{
						temp_board[p][q]=last_board[p][q];	// 复原
					}
				DrawBackground( );	DrawBoard( );							// 复原
				if(depth%2==0)							// 该机器走
				{
					if(a == 0)							// 只是为了确保有解
														// 当然这样也许可以说是不完全的αβ剪枝
														// 因为我的min - max结构本来也不是很正统。
				    {
					    alpha = temp;						// 好吧我没说真话，真话是我的评估函数好夸张能超出整型范围的。
						temp_x = i;
					    temp_y = j;
						comp_x = i;
						comp_y = j;

					    a++;
					    continue;  
				    }
				    if(alpha < temp)
				    {
					    alpha = temp;
						temp_x = i;
						temp_y = j;
				    }
					if(alpha >= beta)
					{
						if(depth == 0)
						{
							comp_x = temp_x;
							comp_y = temp_y;				// 看来，问题是处在这里啊？不爱用指针伤不起……
						}
						return alpha;
					}
				}
                else
				{
					if(beta>temp)						// 人的一方
					{
						beta=temp;
					}
					if(alpha>=beta)
						return beta;
				}
			}  
		}
	}
	if(depth==0)
	{
		comp_x=temp_x;
		comp_y=temp_y;
	}
	if(now==COMPUTER)
	    return alpha;
	else
		return beta;
}