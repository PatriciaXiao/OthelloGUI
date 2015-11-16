
/*
 * othelloAI_Graph.cpp
 *
 *  Created on: 2013-12-5
 *      Author: patriciaxiao
0������dxlib��http://homepage2.nifty.com/natupaji/DxLib/dxdload.html
1���ļ�-���½���Ŀ-��Win32Ӧ�ó���-������Ŀ
2����Ŀ-��������
3����Ŀ-����Ŀ����-��
       1������-����������-��
                    ����������-������-���ַ���-���ַ���-��ʹ�ö��ֽ��ַ���
                    ����������-��C/C++-������-�����Ӱ���Ŀ¼-��
                                    ����ļ��У� \���Ľ�ѹ��Ŀ¼\DxLibVCTest\
                    ����������-��������-������-�����Ӱ���Ŀ¼-��
                                     ͬ��
       2������-��Release-��
                    ��������-��C/C++-����������-�����п�-�����̣߳�/MT��
       3������-��Debug-��
                    ��������-��C/C++-����������-�����п�-�����̵߳��ԣ�/MTd��
*/

// �ڰ��壬ͼ�ν������ض���ͺ���
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

// ��λ����Ϊ0 �������µ�λ����Ϊ1 �������µ�λ����Ϊ2
#define EMPTY		'0'
#define BLACK		'1'
#define WHITE	    '2'
#define MARK		'M'
#define SCALE		 8									// �ڰ����ģһ���� 8 X 8
#define LOW			 1									// �ͼ�������ʵ�������������
#define HIGH		 4									// �߼�����

#define FileNameLEN  60
#define WaitSomeTime 500								// ����ʱ�䣬��������̫��

#define inf          0xfffffff							// ����ƺ�û����ʲô�ó���
#define BOARD_SIZE	 36									// ���ڿ�������ռ�ݻ����С����
#define delta		 6									// ��������λ�õĲ���
#define CIRCLE_Adj	 3.75						        // ���ڸ������̸���������Ӱ뾶���� 	�뾶 = BOARD_SIZE / CIRCLE_Adj 

// ���̳�ʼ����
 
int  BlackColor , WhiteColor ;							// �ڰ���ɫ��������
int  GreenColor , RedColor ;							// ��ɫ���̣���ɫ��Ŀ
int  HumanMark, ComputerMark, MarkPut ;					// ���ڱ���ˡ�������շ�ת��ɫ
int  HUMAN_Color , COMPUTER_Color ;						// ��ѡ�����ɫ����һ��ѡ��ɫ

int human[SCALE][SCALE]={{0}};
int computer[SCALE][SCALE]={{0}};						// �ܷ�����

int comp_x = -1, comp_y = -1;							// �������ӵ㣬��ʼֵ����Խ��,���㿪ʼ��ʱ����ʾ

char HUMAN=BLACK;										// ����Ϊ�˴�����̵Ļ����bug
char COMPUTER=WHITE;
int  DEPTH;												// �������

char board[SCALE][SCALE];
char save_board[SCALE][SCALE];							// ����֮ǰ�Ž��������õ�
char mark_board[SCALE][SCALE];							// ������Ǹձ������������ɫ
int  MarkTime = 0 ;										// ������ʱ�ۿ���תЧ��

// "һ���˵���1(����) 2(����) 3(д��) ESC(�˳�)"	
#define MENU1		1
#define MENU2		2
#define Menu_NONE	6
#define Menu1_New	1
#define Menu1_Read	2
#define Menu1_Save	3
#define Menu1_ESC	0
void PrintMenu_1( );									// ��ӡһ���˵�

// "�����˵���1(����) 2(����) 3(д��) ESC(�˻�)"
#define Menu2_Help	1
#define Menu2_Level	2
#define Menu2_Save	3
#define Menu2_ESC	0
void PrintMenu_2( );									// ��ӡ�����˵�

typedef struct box {
	int x1, y1, x2, y2;
} Box; 

Box oldbox = {0, 480, 0, 480};
Box oldboxYN = {0, 480, 0, 480};

void help( );											// ��ѯ�����ӵ����̸�
void Level( );											// ѡ���������

void change_color(int , int , char, char [SCALE][SCALE], bool);
int judge_put(int ,int , char, char [SCALE][SCALE]);
void possible( );
int mobility(char);
long long find_position(int, char[SCALE][SCALE], long long, long long);

//ÿ��λ�õ�Ȩ��
int const_value[SCALE][SCALE];							// ���в���
int change_value[SCALE][SCALE]={{0}};							// ʱʱ�ڱ�

void put_value( );										// �����̸�Ȩ��

void new_board();										// ���� 2 x 2 ����ĳ�ʼ���
bool ReadBoard( );										// ���������
int  SaveBoard( );										// ����ִ���

#define IllegalChar			1							// �ļ����зǷ��ַ� 
#define NoCentralChess		2							// �м�û������
#define NotConnected		3							// δ����һƬ
#define GoodBoard			0							// ��������

// �������ѡ�����һЩ����
#define CHOICE1_YBL			1							// ��һ��ѡ����1����Y
#define CHOICE2_NWH			2							// �ڶ���ѡ����2����N

#define CHOICE_YN			1							// ѡ��Y��N��
#define CHOICE_BW			2							// ѡ��B��W��
#define CHOICE_LH			3							// ѡ��L��H��

int getchoice(int &, int, char[]);						// �������ͣ���ȡѡ��ֵ		

void DrawBackground( );									// �����̱��������ڱ仯�Ƚϴ��ʱ��
void DrawBoard( );										// �������еĺڰ�����
void MarkChess( int , int , char );						// ��һ���ջ�ɫ���壬��������ʾ��ɫ����
void DrawChess( int, int );								// ��(i, j) ���������в��ô���ɫ������
void ColorChess( int , int , int  );					// ��������ɫ�����������ɫ�����������
void DrawText_1( char [], int );						// �ڵ�һ��д����
void DrawText_2( char [], int );						// �ڵڶ���д����
void DrawText_3( char [], int );						// �ڵ�����д����
int  checkMouse(int &, int &, int);						// �����ĵ���λ��ת��Ϊ���̸����±�ֵ
void Coordinate(int , int, char [ ], int);				// �ѵ���λ��ת�����ַ�����
void PrintScore( );										// ��ӡ��Һͼ�����ĵ÷�
void choose_Color( );									// ѡ��������ɫ����ɫ���ߣ�һ�㶼ѡ����
void start( );											// �������Ҫ���ƺ���
void play( );											// ���ĺ���
int  ClickRange(int , int, int);                        // �˵�ѡ��
int  ClickChoice(int ,int, char [] );                   // "1-2" "Y-N"�����ѡ��

char ChoiceMenuStr[ 128 ] = "                                                                   " ;

// �ж������Ƿ��ʼ����
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
	SetGraphMode( 640 , 480 , 16 ) ; 					// ���Ը����Լ��Ļ����ֱ������ã�����SetGraphMode( 1366 , 768 , 32 );

	if( DxLib_Init() == -1 )							// ��ʼ��DxLib ��
	{
		 return -1;										// ��ʧ���򷵻�-1
	}

	WhiteColor = GetColor( 255 , 255 , 255 ) ;			// ��ɫ
	BlackColor = GetColor( 0 , 0 , 0 );					// �\ɫ
	GreenColor = GetColor( 0 , 120 , 0 );				// ��ɫ
	RedColor = GetColor( 200 , 60 , 60 );				// ��ɫ

	DEPTH = HIGH;										// ������ȣ��ȶ�Ϊ�߼�
	HumanMark = BlackColor;								// ��ʼΪ�߼����仯ɫͬ��ɫ
	MarkPut = ComputerMark = WhiteColor;				// ��ʼΪ�߼����仯ɫͬ��ɫ

	new_board( );										// �ȳ�ʼһ������ 2 x 2 ����	
	put_value( );										// �����λ�ø�Ȩ��

	SetMouseDispFlag( TRUE ) ;							// �������Ϊ�ɼ�
	DrawBackground( );
	DrawBoard( );										// ���� 
	start(  ) ;											// ϵͳ�ܿأ��ȴ�����˵�ѡ�� 	
	DxLib_End( ) ;										// DxLib���˳�����
	return 0 ;											// �˳�����
}

void start(  )											// �������Ҫ���ƺ���
{
	int j, i  ;											// �кš��к�
	int click_menu ;                                
	int click, choice_menu;
	int fileOP = false;

	PrintMenu_1( );	 									// ��ӡһ���˵�
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

			DrawBox(oldbox.x1 , oldbox.y1 , oldbox.x2 , oldbox.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;	// ��Ӱ
			PrintMenu_1( );

			switch (click_menu) {
				case Menu1_New :							// ������
									new_board( );
									DrawBackground( );
									DrawBoard( );
									DrawText_2( "���¿�ʼ��Ϸ������µ�����B/W��ѡ����ɫ ", WhiteColor); 
									choose_Color( );
									play( );
									break;
				case Menu1_Read:							// ������
									fileOP = ReadBoard( ); 
									if ( Is_initialboard( ) )
										continue;			// �����ԭʼ2 X 2���̣���������������ѡ��
									play( );				// ����ɹ������������ļ�����������������̣���������
									break ;
				case Menu1_Save:							// ������
									fileOP = SaveBoard( );
									if ( Is_initialboard( ) )
										continue;			// �����ԭʼ2 X 2���̣���������������ѡ��
									DrawText_2( "��Ϸ����������µ����ѡ������         ", WhiteColor); 
									play( );
									break;
				case Menu1_ESC:								// Escape �˳���Ϸ
									while (1)
									{
										strcpy(ChoiceMenuStr, "�˳�ѡ��(Y/N)��Y.�˳���Ϸ   N.���˳�(����)");
										DrawText_1(ChoiceMenuStr, WhiteColor );

										click = getchoice(choice_menu, CHOICE_YN, ChoiceMenuStr) ;
										if (!click || 
											((choice_menu != CHOICE1_YBL) && (choice_menu != CHOICE2_NWH))) 
											continue;

										if ( choice_menu == CHOICE1_YBL ) 
											return; 
										else
										{
											DrawText_2( "����ѡ���˳�������¼�����Ϸ     ", WhiteColor);	
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
	int click_menu = Menu_NONE , click = false, choice_menu = CHOICE1_YBL;	// ����ֵ������������

	PrintMenu_2( );										// ��ӡ�����˵�
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
			DrawText_2( "�԰����꣬������˼��          ", WhiteColor ); 			
			now=COMPUTER;
		}

		if ( mobility(now) == 0 )
		{
			if (steps%2 == 0) 
				lstrcpy(  ChoiceMenuStr  , "���������£��������壡����°����������" ) ;
			else lstrcpy(  ChoiceMenuStr  , "����������£��������壡����°����������" );
			DrawText_1( ChoiceMenuStr, WhiteColor );
			WaitTimer(WaitSomeTime);					// ���������wait, �ͻ�һ������
			while( CheckHitKeyAll() == 0 )				// �ȴ��û����������
			{
			}
	
			steps++;
			continue;
		}

		if (now == HUMAN)								// ��������
		{
			if (steps != 0)
			{
				lstrcpy( StrBuf , "�ֵ������ˣ������������� " ) ;	// StrBuf ��׼������
				Coordinate(comp_x, comp_y, StrBuf2, 1 );	// ����������ַ���
				lstrcat( StrBuf , StrBuf2 ) ; 			// StrBuf ��StrBuf2 ƴ��
				DrawText_2( StrBuf, WhiteColor ); 
			}

			//��δ����Ϣ
			int out;
			while( ProcessMessage() == 0 )
			{
				out = false;
				click = checkMouse( i , j, MENU2 ) ;			// ��ȡ��갴��λ��
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

				DrawBox(oldbox.x1 , oldbox.y1 , oldbox.x2 , oldbox.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;	// ��Ӱ
				// PrintMenu_2( );							��������Ϊ�˱����»��߸��Ƕ����»��˵�

				switch (click_menu) {
					case Menu2_Help : 
										help( );		// ������ҿ����ӵ�λ��
										continue;
					case Menu2_Level: 
										Level( );
										continue;
					case Menu2_Save : 
										SaveBoard( );
										continue;
					case Menu2_ESC : 
										// Escape �˳���Ϸ
										lstrcpy(ChoiceMenuStr, "�˳�ѡ��(Y/N)��Y.�˻��ϲ�   N.������(����)");
										DrawText_1(ChoiceMenuStr , WhiteColor );
										WaitTimer(WaitSomeTime);	// ���������wait, ��һ������
										click = getchoice(choice_menu, CHOICE_YN, ChoiceMenuStr ) ;
										if (!click || ((choice_menu != CHOICE1_YBL) && (choice_menu != CHOICE2_NWH)))
											continue;

										if ( choice_menu == CHOICE1_YBL )
											return; 
										else 
											DrawText_2( "����ѡ���˳����������Ϸ     ", WhiteColor);	
										break;
					default : 	
										break;
				}

				if(!out )
				if (( ( i >= 0 ) && ( i < SCALE ) && ( j >= 0 ) && ( j < SCALE ) )||(click_menu == 4))
				{
					if (!human[i][j]) 
						continue;

					if (board[i][j] != EMPTY)			// �˿�������
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
			DrawText_2( "�԰����꣬��������˼��          ", WhiteColor ); 	
			find_position(0,board,-inf,inf);
			steps++;
			change_color(comp_x, comp_y, now,board, true);  

			DrawBoard( );
			ColorChess( comp_x, comp_y, MarkPut );
		}
	}
	PrintScore( );
	PrintMenu_1( );										// ��ӡһ���˵�
}

void PrintMenu_1( )										// ��ӡһ���˵�
{
	DrawText_3( "һ���˵���1(����) 2(����) 3(д��) ESC(�˳�)", WhiteColor); 		
}

void PrintMenu_2( )										// ��ӡ�����˵�
{
	DrawText_3( "�����˵���1(����) 2(����) 3(д��) ESC(�˻�)", WhiteColor); 		
}

// "һ���˵���1(����) 2(����) 3(д��) ESC(�˳�)"								
// "�����˵���1(����) 2(����) 3(д��) ESC(�˻�)"
int ClickRange(int x,int y, int menuLevel)				// �˵����ѡ��
{
	int range ;
	int y1 = 432, y2 = 454 , x1 = 0, x2 = 85;

	if((y > 452)||(y < 432)) 
		 range = Menu_NONE;								// ��Ч�������
	else if((x >= 93)&&(x <=160))
	{
		range = 1 ;										// 1���˵�
		x1 = 93;	x2 = 160;
	}
	else if((x >= 172)&&(x <= 238))
	{
		range = 2 ;										// 2���˵�
		x1 = 172;	x2 = 238;
	}
	else if((x >= 249)&&(x <= 316))
	{
		range = 3 ;										// 3���˵�
		x1 = 249;	x2 = 316;
	}
	else if((x >= 326)&&(x <= 416))
	{
		range = 0 ;										// ESC���˵�
		x1 = 326;	x2 = 416;
	}
	else range = Menu_NONE ;

	DrawBox(oldbox.x1 , oldbox.y1 , oldbox.x2 , oldbox.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;
	DrawBox(x1 - 2 , y2 , x2 + 2, y2 + 4 ,  GetColor( 0 , 200 , 0 ) , TRUE ) ;
//  ��������Ϊ�˲����»����Ƿ�Ӱ���˲˵�ѡ�����ݵġ����ڲ���Ҫ��
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

	if((y > 382)||(y < 357))							// ��Ч�������
		range = Menu_NONE;					
	else if((x >= 145) && (x <= 250))
	{
		range = CHOICE1_YBL;
		x1 = 145;	x2 = 250 ;							// 1��ѡ��
	}
	else if((x > 275) && (x <= 400))
	{
		range = CHOICE2_NWH ;							// 2��ѡ��
		x1 = 275;	x2 = 400 ;
	}
	else range = Menu_NONE ;

	DrawBox(oldboxYN.x1 , oldboxYN.y1 , oldboxYN.x2 , oldboxYN.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;
	DrawBox(x1 - 2 , y2, x2 + 2, y2 + 4 ,  GetColor( 0 , 200 , 0 ) , TRUE ) ;
	// DrawText_1( ChoiceMenuStr , WhiteColor );		��������Ϊ�˲����»����Ƿ�Ӱ���˲˵�ѡ�����ݵġ����ڲ���Ҫ��

	oldboxYN.x1 = x1 - 2; oldboxYN.y1 = y2; 
	oldboxYN.x2 = x2 + 2; oldboxYN.y2 = y2 + 4;

	return range ;
}

void Level( )
{
	int choice_menu;
	int click;

	lstrcpy( ChoiceMenuStr , "����ѡ��(L/H)��L.����(��ת) H.�߼�(���Ĳ�)");
	DrawText_1( ChoiceMenuStr, WhiteColor );

	while (1)
	{
		click = getchoice(choice_menu, CHOICE_LH, ChoiceMenuStr) ;
		if (!click || ((choice_menu != CHOICE1_YBL) && (choice_menu != CHOICE2_NWH)))
			continue;

		if( choice_menu == CHOICE1_YBL ) 
		{
			DrawText_2(  "�ͼ�����������ʾ��ɫ��ת���������������죩", WhiteColor); 
			DEPTH = LOW;
			MarkTime = WaitSomeTime;						// �ͼ��壬�����㹻ʱ�䣬����۲�仯
			HumanMark = GetColor( 0 , 120 , 120 );			// ���ǳ��ɫ�����˸ձ仯��
			ComputerMark = GetColor( 100 , 20 , 20 );		// ���ǳ��ɫ���Ǽ�����ձ仯��
			MarkPut = RedColor = GetColor( 200 , 20 , 20 );	// ���ڱ�Ǽ�������µ���ɫ����ɫ
			break;
		}
		else 
		{
			DrawText_2(  "�߼����������ڰ���Ϸ����ʾ��ɫ��ת              ", WhiteColor); 
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

void help( )											// ��ѯ�����ӵ����̸�
{
	int i, j, count = 0;								// ���������5���͹���
	char StrBuf[128], StrBuf2[28];
	lstrcpy( StrBuf , "�����Ӵ�" ) ;					// StrBuf �д洢�˵�

	possible();
	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
			if(human[i][j])								// ��������
			{
				Coordinate( i, j, StrBuf2 ,1);
				lstrcat( StrBuf , StrBuf2 ) ; 			// StrBuf ��StrBuf2 ƴ��
				DrawText_2( StrBuf, RedColor ); 		
				
				count++;
				if (count >= 5)
					return;
			}
	if (count == 0)
	{
		lstrcpy( StrBuf , "����޴�������        " ) ;	// StrBuf �д洢�˵�
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
	// ��ʼ���
	board[SCALE/2-1][SCALE/2-1] = WHITE;
	board[SCALE/2][SCALE/2] = WHITE;
	board[SCALE/2-1][SCALE/2] = BLACK;
	board[SCALE/2][SCALE/2-1] = BLACK;
	return;
/*  �൱��
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, W, B, 0, 0, 0 } ,
	{ 0, 0, 0, B, W, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ,
	{ 0, 0, 0, 0, 0, 0, 0, 0 } ; */
}

void DrawBackground( )									// �����̱��� 
{
	int i, j;											// �С�������
	char StrBuf[128], ch[2];

	lstrcpy( StrBuf , "     A   B  C   D  E   F  G   H  " );
	DrawString( 0 , 0, StrBuf ,  WhiteColor ) ;
	DrawString( 0, (SCALE+1) * BOARD_SIZE + 9, StrBuf, WhiteColor); 
		
	// ���Աߵ����� 0 - 7
	for( i = 1 ; i <= SCALE ; i ++ ) {
		ch[0] = (i-1) + '0';  ch[1] = '\0';
		lstrcpy( StrBuf , ch);
		lstrcat( StrBuf , "                                 " );
		lstrcat( StrBuf , ch);
		DrawString( 5, i * BOARD_SIZE + 9 , StrBuf, WhiteColor); 
	}
	
	// �����̵�ɫ
	DrawBox( BOARD_SIZE , BOARD_SIZE - delta ,(SCALE+1) * BOARD_SIZE ,  (SCALE+1) * BOARD_SIZE - delta, 
				GreenColor , TRUE ); 
	// ������
	for( i = 2 ; i <= SCALE ; i ++ )
		DrawLine(BOARD_SIZE , i * BOARD_SIZE - delta, (SCALE+1) * BOARD_SIZE,  i * BOARD_SIZE - delta,  GetColor( 20 , 20 , 20) ) ;
	// ������
	for( j = 2 ; j <= SCALE ; j ++ )
		DrawLine(j * BOARD_SIZE , BOARD_SIZE - delta,  j * BOARD_SIZE , (SCALE+1) * BOARD_SIZE - delta,  GetColor( 20 , 20 , 20) ) ;

	/*
	// �����̵�ɫ
	DrawBox( BOARD_SIZE , BOARD_SIZE ,(SCALE+1) * BOARD_SIZE ,  (SCALE+1) * BOARD_SIZE , 
				GreenColor , TRUE ); 
	// ������
	for( i = 2 ; i <= SCALE ; i ++ )
		DrawLine(BOARD_SIZE , i * BOARD_SIZE , (SCALE+1) * BOARD_SIZE,  i * BOARD_SIZE ,  GetColor( 20 , 20 , 20) ) ;
	// ������
	for( j = 2 ; j <= SCALE ; j ++ )
		DrawLine(j * BOARD_SIZE , BOARD_SIZE,  j * BOARD_SIZE , (SCALE+1) * BOARD_SIZE,  GetColor( 20 , 20 , 20) ) ;
	*/
}

void DrawBoard( )										// ������ 
{
	int i, j;											// �С�������
	for( i = 0 ; i < SCALE ; i ++ )
		for( j = 0 ; j < SCALE ; j ++ )
			if (board[ i ][ j ] != EMPTY ) 
				DrawChess( i, j );
}

void DrawChess( int i , int j ) 
{
	int size, chesscolor ;
	switch(board[i][j]) {
		case EMPTY:		break;							// �ո���
		case BLACK:		chesscolor = BlackColor;		// ���� GetColor( 0 , 0 , 0 );					
						break;
		case WHITE:	    chesscolor = WhiteColor;		// ���� GetColor( 255 , 255 , 255 ) ;					
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

void DrawText_1( char StrBuf[], int color )				// ��յ�һ����ʾ�����ٴ�ӡ����ʾ
{
	DrawBox( 0 , (SCALE + 2) * BOARD_SIZE, 639 ,  (SCALE + 2) * BOARD_SIZE + 24 , BlackColor , TRUE ) ;
	DrawString( 0 , (SCALE + 2) * BOARD_SIZE + 2, StrBuf , color ) ;
}

void DrawText_2( char StrBuf[], int color )				// ��յڶ�����ʾ�����ٴ�ӡ����ʾ
{
	DrawBox(0 , ( SCALE + 3 ) * BOARD_SIZE, 639 ,  ( SCALE + 3 )  * BOARD_SIZE + 20 , BlackColor , TRUE ) ;
	DrawString( 0 , ( SCALE + 3 ) * BOARD_SIZE + 2, StrBuf , color ) ;
}

void DrawText_3( char StrBuf[], int color )				// ��յ�������ʾ�����ٴ�ӡ����ʾ
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
	lstrcat( StrBuf , ch ) ;							// StrBuf ��ch ƴ��
	lstrcat( StrBuf , ", " ) ;							// StrBuf ��ch ƴ��
	_itoa( j , StrBuf2 , 10 ) ;							// MouseY ֵת������������ִ���StrBuf2
	lstrcat( StrBuf , StrBuf2 ) ; 						// StrBuf ��StrBuf2 ƴ��
	lstrcat( StrBuf , ") " ) ;
}

int  checkMouse( int &i , int &j , int menuLevel )
{
	char StrBuf[ 128 ] , StrBuf2[ 32 ] ;
	int MouseInput ;
	int MouseX , MouseY ;

	// �������
	while( CheckHitKeyAll() == 0 )
	{
		GetMousePoint( &MouseX , &MouseY ) ;			// ��ȡ���λ��

		i = int ( MouseX / BOARD_SIZE ) - 1;	
		j = int (( MouseY + delta ) / BOARD_SIZE ) - 1;

		lstrcpy( StrBuf , "���λ�ã�" ) ;
		if( i<8 && j<8 && i>=0 && j>=0 )
		    Coordinate( i, j, StrBuf2 ,1);
		else
		{
			Coordinate( MouseX, MouseY, StrBuf2 , 0);
		}
		lstrcat( StrBuf , StrBuf2 ) ; 					// StrBuf ��StrBuf2 ƴ��

		MouseInput = GetMouseInput() ;
		DrawText_1(StrBuf, WhiteColor); 
		
		// �ж��Ƿ�������ȷ������
		if ( ( MouseInput && MOUSE_INPUT_LEFT ) == 0 )
		{
			if( ProcessMessage() == -1 )				// �¼�����,�������
				break ;									// �������
		}

		ClickRange(MouseX, MouseY, menuLevel);
		
		WaitTimer( 100 ) ;								// �ȴ�һ��ʱ�䣬������Ļ�����ֵ�仯̫��
	}

	if(( MouseInput && MOUSE_INPUT_LEFT ) == 0)			// �����
		return true;
	return false ;										// ��������
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

	lstrcpy( StrBuf , "��ң�" ) ;						// StrBuf ��׼��"��ҵ÷�" ������
	_itoa( countHuman , StrBuf2 , 10 ) ;				// ��ҵ÷�ֵת�ɺ���������ִ���StrBuf2
	lstrcat( StrBuf , StrBuf2 ) ;						// StrBuf ��StrBuf2 ƴ��
	lstrcat( StrBuf , "  �������" ) ;					// StrBuf ��������"������÷�"
	_itoa( countComputer , StrBuf2 , 10 ) ;				// ������÷�ֵת������������ִ���StrBuf2
	lstrcat( StrBuf , StrBuf2 ) ; 						// StrBuf ��StrBuf2 ƴ��

	if ( countHuman < countComputer)
		lstrcat( StrBuf , " ���ź�~��������~��");
	else if(countHuman > countComputer )
		lstrcat( StrBuf ,"  ���ģ���������ˣ�\n");
	else
		lstrcat( StrBuf , " ƽ�ְ�~~~�ѷ�ʤ��");
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
	// �������
	while( CheckHitKeyAll() == 0 )
	{
		GetMousePoint( &MouseX , &MouseY ) ;			// ��ȡ���λ��
		MouseInput = GetMouseInput() ;

		// �ж��Ƿ�������ȷ������
		if ( ( MouseInput && MOUSE_INPUT_LEFT ) == 0 )
		{
			if( ProcessMessage() == -1 ) {				// �¼�����,�������
				break ;									// �������
			}
		}
		ClickChoice(MouseX, MouseY,  ChoiceMenuStr );
		WaitTimer( 100 ) ;								// �ȴ�һ��ʱ�䣬������Ļ�����ֵ�仯̫��
	}

	// ��������
	if ( CheckHitKey( Choice1 ) != 0 )					// YBL��
		click_menu = CHOICE1_YBL;
	else if (( CheckHitKey( Choice2 ) != 0 ) )			// NWH��
		click_menu = CHOICE2_NWH;
	else if(( MouseInput && MOUSE_INPUT_LEFT ) == 0) 	// �����
	{
		GetMousePoint( &i , &j );
		click_menu = ClickChoice( i , j, str );
	}
	else click = false;									// ѡ��������������Ϊ��

	DrawBox(oldboxYN.x1 , oldboxYN.y1 , oldboxYN.x2 , oldboxYN.y2 ,  GetColor( 0 , 0 , 0 ) , TRUE ) ;	// ��Ӱ
	DrawText_1(  ChoiceMenuStr , WhiteColor );
	return click;
}

void choose_Color( )
{
	int choice_menu;
	int click;

	lstrcpy(ChoiceMenuStr, "��ɫѡ��(B/W)��B.����(����) W.����(������)");
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
			DrawText_2( "�������֣���������                  ", WhiteColor); 
			DrawBoard();
			break;									
		}
		else if ( choice_menu == CHOICE2_NWH ) 
		{
			HUMAN=WHITE;
			MarkPut=COMPUTER=BLACK;						// �����޸�markput���Ǻֲܿ���
			HumanMark = WhiteColor;						
			MarkPut = ComputerMark = BlackColor;
			possible( );
			DrawText_2(  "���Ӻ��֣����ɸ���ǫ�ã����²�������", WhiteColor); 

			srand((unsigned)time(NULL));				// ��ʼ��
			int a=rand();								// �����
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

int SaveBoard()										// ���ļ���д��������
{
	int i, j, k = 0;                          
	int click;
	int choice_menu; 
	char StrBuf[ 128 ] ;				// ��ʾ��Ϣ�õĻ���

	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;  // �Ǻǡ�����ô���ĳ��򶼱�����ˣ����һ��������������鳤��������ջ�������

	ch = '0';
	DrawText_1(  "д�̡�����������ļ�����<30���� " , WhiteColor );
	DrawText_2( "                                             ", WhiteColor); 

	KeyInputString( 0 , ( SCALE + 3 ) * BOARD_SIZE, 639 , filename , FALSE ); // gets(filename);
	if (_access(filename, 0) == 0)   // �Ѿ�����
	{
		lstrcpy( StrBuf , filename ) ;	
		lstrcat( StrBuf , "�ļ��Ѿ�����                 " ) ;	
		DrawText_2( StrBuf, WhiteColor);
		lstrcpy( ChoiceMenuStr, "����ѡ��(Y/N)�� Y.�����ļ�  N.�������ļ�" );
		DrawText_1( ChoiceMenuStr, WhiteColor);	

		while( !ProcessMessage() )
		{
			click = getchoice(choice_menu, CHOICE_YN, ChoiceMenuStr ) ;
			if (!click || ((choice_menu != CHOICE1_YBL) && (choice_menu != CHOICE2_NWH)))
				continue;

			if ( choice_menu == CHOICE2_NWH )
			{											// ѡ��N
				DrawText_2( "����ѡ�񲻸����ļ����������Ϸ��  ", WhiteColor );
				return false;
			}
			else break ;
		}
	}

	err  = fopen_s( &fp, filename, "w+" );

	if( err != 0 )
	{
		lstrcpy( StrBuf , filename ) ;	
		lstrcat( StrBuf , "�ļ���ʧ��              " );
		DrawText_2( StrBuf , WhiteColor ); 
		return false;
	}
	else {
		lstrcpy( StrBuf , filename ) ;
		lstrcat( StrBuf , "�ļ��ɹ����棬����¼�����Ϸ   " );
		DrawText_2( StrBuf, WhiteColor ); 
	}

	for(j=0;j<SCALE;j++)								// ����
	{
		for(i=0;i<SCALE;i++) 
			str[k++] = board[i][j];
		str[k++]='\n';									// �ӻس�
	}
	str[k++]=COMPUTER; 
	str[k++]=HUMAN;

    fwrite(str,1,(SCALE+1)*SCALE+2,fp);	// ����ԭ��size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
										// ע�⣬���� str��û�б���"\0"�ַ�����������������    fwrite(str,1,strlen(str),fp);	
    fclose(fp);											// �ǵù��ϡ���
	return true;
}

//�ж��Ƿ�Ϸ����̵�ϵ�к���

//�ݹ�ר��
void walk(int i, int j, int b[SCALE][SCALE])
{
	if((i<0)||(i>=SCALE)||(j<0)||(j>=SCALE))
		return;											// ���緵��
	if(!b[i][j])										// ��ֵ����
		return;
	b[i][j]=0;
	walk(i-1,j-1,b);									// ��˷�����
	walk(i-1,j,b);
	walk(i-1,j+1,b);
	walk(i,j-1,b);
	walk(i,j+1,b);
	walk(i+1,j-1,b);
	walk(i+1,j,b);
	walk(i+1,j+1,b);
}

//�ж�����
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
				else								// ���ڷǷ��ַ�
					return IllegalChar;
		}
	}
	for(i=SCALE/2-1;i<=SCALE/2;i++)
	{
		for(j=SCALE/2-1;j<=SCALE/2;j++)
			if(!temp[i][j])							// �������û�����ӣ�һ���Ƿ�
				return NoCentralChess;
	}
	walk(SCALE/2,SCALE/2,temp);
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
			if(temp[i][j])
				return NotConnected;				// δ����һƬ��һ������
	}
	return GoodBoard;
}

bool ReadBoard()										// ���ļ��ж�����������
{
	int i, j, k = 0;
	char StrBuf[ 128 ] ;								// ��ʾ��Ϣ�õĻ���

	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;  // �Ǻǡ�����ô���ĳ��򶼱�����ˣ����һ��������������鳤��������ջ�������
	int  board_status;
	 
	ch='0';

	DrawText_1( "���̡�����������ļ�����<30���� ", WhiteColor );
	DrawText_2( "                                              ", WhiteColor); 

	KeyInputString( 0 , ( SCALE + 3) * BOARD_SIZE, 639 , filename , FALSE ); // gets(filename);
	err  = fopen_s( &fp, filename, "r" );
	if( err != 0 ) {
		lstrcpy( StrBuf , filename ) ;	
		lstrcat( StrBuf , " �ļ���ʧ�ܡ��밴���������" );
		DrawText_2( StrBuf, WhiteColor ); 
		WaitTimer(WaitSomeTime);						// ���������wait, �ͻ�һ������
		while( CheckHitKeyAll() == 0 )					// �ȴ��û����������
		{
		}
		return false;
	}

	fread(str,1,SCALE*(SCALE+1)+2,fp);	
	// ����ԭ��size_t fread(void *buffer, size_t size, size_t count, FILE *stream);��

	for (j=0;j<SCALE;j++)
	{
		for(i=0;i<SCALE;i++)  {
			save_board[i][j] = board[i][j]; 
			board[i][j] = str[k++];
		}
		k++;											// ȥ���س�
	}
	char computer=str[k++];
	char human=str[k++];

    fclose(fp);											// �ù��͹غ�ϰ��

	board_status = board_check( board );
										
	switch (board_status) {
		case IllegalChar :
							lstrcpy( StrBuf , filename ) ;
							lstrcat( StrBuf , " �����޷�ʶ��ķ��š��밴���������" );
							break;
		case NoCentralChess:
							lstrcpy( StrBuf , filename ) ;
							lstrcat( StrBuf , " ������û���������ӡ��밴���������" );	
							break;
		case NotConnected :
							lstrcpy( StrBuf , filename ) ;
							lstrcat( StrBuf , " ���������Ӳ��������밴���������" );
							break;
		case GoodBoard :
							HUMAN = human;
							COMPUTER = computer;
							lstrcpy( StrBuf , filename ) ;
							lstrcat( StrBuf , " �����ļ��ɹ����룬" ) ;
							if ( HUMAN == BLACK) 
								lstrcat(StrBuf, "���ִ����") ;
							else lstrcat(StrBuf, "���ִ����") ;
		default :
							break;
	}

	DrawText_2( StrBuf, WhiteColor ); 
	if (board_status != IllegalChar) {
		DrawBackground( );	DrawBoard( );					// ���¶�����ļ���������
	}
	WaitTimer(WaitSomeTime);								// ���������wait, �ͻ�һ������
	while( CheckHitKeyAll() == 0 )							// �ȴ��û����������
	{
	}

	if (board_status != GoodBoard) 
	{
		for (j=0;j<SCALE;j++)
			for(i=0;i<SCALE;i++)  
				board[i][j] = save_board[i][j];
		lstrcpy( StrBuf , "�ļ�����ʧ�ܣ�����¼�����Ϸ" );
		DrawText_2( StrBuf, WhiteColor ); 
		DrawBackground( ); DrawBoard( );					// ��ԭ
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

//�ж��ܷ����ӵĺ���
int judge_put(int i, int j, char whoseturn, char board[SCALE][SCALE])
{
    if(board[i][j]!=EMPTY)								// �����ص�����
		return 0;
	char against;
	if(whoseturn==COMPUTER)
		against=HUMAN;
	else
		against=COMPUTER;
	int k;
	if(board[i][j]!=EMPTY)								// �����ص�����
		return 0;
	//��
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
	//��
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
	//��
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
	//��
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
	//����
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
	//����
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
	//����
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
	//����
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

//�����ܷ�������Ϣ�ĺ���
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

//�ж��ж����ĺ���
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
	else  // bug= =bû�Ӵ����ţ��Ҿ�������һ�£������𡭡���
	{
		for(i=0;i<SCALE;i++)
			for(j=0;j<SCALE;j++)
				if(human[i][j])
					n++;
	}
	return n;
}

//����֮��ı����̵ĺ���
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
	//��
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
	//��
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
	//��
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
	//��
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
	//����
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
	//����
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
	//����
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
	//����
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


//Ѱ�����ŵ������ϵ�к���

//�����ȶ�ϵ���ĺ���
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
	//����
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
	//����
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
	//��������
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
	// ��������
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


//��ֵ����
//����ǰ��ֹ�ֵ
//Ѱ�����ŵ������ϵ�к���

void c_val(char board[SCALE][SCALE])   //����ɱ�Ȩ��
{
	int i,j;
	int n=SCALE-1;
	for(i=0;i<=n;i++)
		for(j=0;j<=n;j++)
			change_value[i][j]=0;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))	// ��
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))	// ��
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))	// ��
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))	// ��
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	// �Է�ռ�˽ǵ���������Դ���
	if(board[0][0]==HUMAN)
	    change_value[0][1]=change_value[1][0]=change_value[1][1] =- 300;
	if(board[0][SCALE-1]==HUMAN)
	    change_value[0][SCALE-2]=change_value[1][SCALE-2]=change_value[1][SCALE-1] =- 300;
	if(board[SCALE-1][0]==HUMAN)
	    change_value[SCALE-1][1]=change_value[SCALE-2][0]=change_value[SCALE-2][1] =- 300;
	if(board[SCALE-1][SCALE-1]==HUMAN)
	    change_value[SCALE-1][SCALE-2]=change_value[SCALE-2][SCALE-1]=change_value[SCALE-2][SCALE-2] =- 300;
	// �Է�ռ�ߵĲ��Ա仯�ִ���
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
	//����ռ�ǵ�������Դ���
	int fight = 1000;
	//origin:300
	//���Ͻ�
	if(board[0][1]!=EMPTY)
		change_value[0][0]+=fight;
	if(board[1][0]!=EMPTY)
		change_value[0][0]+=fight;
	if(board[1][1]!=EMPTY)
		change_value[0][0]+=fight;
	//���Ͻ�
	if(board[0][SCALE-2]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	if(board[1][SCALE-2]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	if(board[1][SCALE-1]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	//���½�
	if(board[SCALE-1][1]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	if(board[SCALE-2][1]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	if(board[SCALE-2][0]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	//���½�
	if(board[SCALE-2][SCALE-2]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
	if(board[SCALE-1][SCALE-2]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
	if(board[SCALE-2][SCALE-1]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
}


//��ֵ����
//����ǰ��ֹ�ֵ

int temp_value(char board[SCALE][SCALE])		// �Ե��и߼��в�ͬ�ļ���Ȩ�ط���
{
	for(int i = 0 ; i < SCALE ; i++)
		for(int j = 0 ; j < SCALE ; j++)
			change_value[i][j] = 0;
	if ( DEPTH == HIGH )						// �������еͼ��Ͳ����㶯̬Ȩ�أ�ֻ����߼���
		c_val(board);
	int score=0;
	int i,j;
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(board[i][j]==COMPUTER)  
			{
				if(DEPTH != LOW)		// ���ͼ��������ڼ�����µ�ʱ�򲻼����ȶ��ԡ�����ҿ���
					score+=steady(i,j,COMPUTER,board)*30;	// �ȶ���
				score+=(const_value[i][j]+change_value[i][j]);      //λ�ù���Ȩ��+�ɱ�Ȩ��
			}
			else
				if(board[i][j]==HUMAN)
				{
					score-=steady(i,j,HUMAN,board)*30;
					score-=(const_value[i][j]-change_value[i][j]);  //ע��change_valueȫ���Լ�����Ƕ�Ϊ׼�ģ�
				}
			if(DEPTH != LOW)
			{
				if(judge_put(i,j,COMPUTER,board))			// �ж���
					score+=100;
				if(judge_put(i,j,HUMAN,board))
					score-=100;
			}
			else
			{
				int num_c = 1000; 
				int num_h = 1000;
				//origin:400//origin:500//origin:600
				if(judge_put(i,j,COMPUTER,board))			// �ж���
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

// ��alpha-beta��֦
long long find_position(int depth, char last_board[SCALE][SCALE], long long alpha, long long beta)
{
	long long temp;										// ����
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
			if(judge_put(i,j,now,last_board))			// ��������֮��
			{
				change_color(i,j,now,temp_board,false);
				temp=find_position(depth+1, temp_board, alpha, beta);
				for(p=0;p<SCALE;p++)
					for(q=0;q<SCALE;q++) 
					{
						temp_board[p][q]=last_board[p][q];	// ��ԭ
					}
				DrawBackground( );	DrawBoard( );							// ��ԭ
				if(depth%2==0)							// �û�����
				{
					if(a == 0)							// ֻ��Ϊ��ȷ���н�
														// ��Ȼ����Ҳ�����˵�ǲ���ȫ�Ħ��¼�֦
														// ��Ϊ�ҵ�min - max�ṹ����Ҳ���Ǻ���ͳ��
				    {
					    alpha = temp;						// �ð���û˵�滰���滰���ҵ����������ÿ����ܳ������ͷ�Χ�ġ�
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
							comp_y = temp_y;				// �����������Ǵ������ﰡ��������ָ���˲��𡭡�
						}
						return alpha;
					}
				}
                else
				{
					if(beta>temp)						// �˵�һ��
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