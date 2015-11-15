
/*
 * othelloai.cpp
 *
 *  Created on: 2013-1-5
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
#define LOW			 2									// �ͼ�������ʵ�������������
#define HIGH		 4									// �߼�����

#define FileNameLEN  60

#define inf          0xfffffff							// ����ƺ�û����ʲô�ó���
#define BOARD_SIZE	 36									// ���ڿ�������ռ�ݻ����С����
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

void PrintMenu_1( );									// ��ӡһ���˵�
void PrintMenu_2( );									// ��ӡ�����˵�
void help( );											// ��ѯ�����ӵ����̸�
void Level( );											// ѡ���������

void change_color(int , int , char, char [SCALE][SCALE], bool);
int judge_put(int ,int , char, char [SCALE][SCALE]);
void possible( );
int mobility(char);
long long find_position(int, char[SCALE][SCALE], long long, long long);

//ÿ��λ�õ�Ȩ��
int const_value[SCALE][SCALE];							// ���в���
int change_value[SCALE][SCALE];							// ʱʱ�ڱ�

void put_value( );										// �����̸�Ȩ��

void new_board();										// ���� 2 x 2 ����ĳ�ʼ���
bool ReadBoard( );										// ���������
void SaveBoard( );										// ����ִ���
void getchoice(char &, char, char, char []);			// ��ȡѡ��ֵ
void DrawBackground( );									// �����̱��������ڱ仯�Ƚϴ��ʱ��
void DrawBoard( );										// �������еĺڰ�����
void MarkChess( int , int , char );						// ��һ���ջ�ɫ���壬��������ʾ��ɫ����
void DrawChess( int, int );								// ��(i, j) ���������в��ô���ɫ������
void ColorChess( int , int , int  );					// ��������ɫ�����������ɫ�����������
void DrawText_1( char [], int );						// �ڵ�һ��д����
void DrawText_2( char [], int );						// �ڵڶ���д����
void DrawText_3( char [], int );						// �ڵ�����д����
void checkMouse(int &, int &);							// �����ĵ���λ��ת��Ϊ���̸����±�ֵ
void Coordinate(int , int, char [ ]);					// �ѵ���λ��ת�����ַ�����
void PrintScore( );										// ��ӡ��Һͼ�����ĵ÷�
void choose_Color( );									// ѡ��������ɫ����ɫ���ߣ�һ�㶼ѡ����
void start( );											// �������Ҫ���ƺ���
void play( );											// ���ĺ���

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
	put_value();										// �����λ�ø�Ȩ��

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
	char ch; 

	PrintMenu_1( );										// ��ӡһ���˵�
	while(1) {	
		while( ProcessMessage() == 0 )
		{
			checkMouse( i , j ) ;
			if ( CheckHitKey( KEY_INPUT_ESCAPE ) != 0 )	{	// Escape �˳���Ϸ
				DrawText_1( "���ǵ������˳���Ϸô����Y/N��", WhiteColor );
				getchoice (ch,'y', 'n', "�˳�");
				if (ch == 'y') {
					return; 
				}
				else continue; 
			}
			if ( CheckHitKey( KEY_INPUT_1 ) != 0 )	{	// ����
				if (ReadBoard( )) {
					choose_Color( );
					play( );
				}
			}
			if ( CheckHitKey( KEY_INPUT_2 ) != 0 )	{	// д�̣�������ǰ������
				SaveBoard( );
				DrawText_2( "��Ϸ���������µ����ѡ������           ", WhiteColor); 
				play( );
			}
			if ( CheckHitKey( KEY_INPUT_0 ) != 0 ) {	// �¿�ʼ��ѡ��ɫ
				new_board( );
				DrawBackground( );
				DrawBoard( );
				choose_Color( );
				play( );
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
	char ch;
	char StrBuf[ 128 ] , StrBuf2[ 32 ];

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
			if (steps%2 == 0) {
				DrawText_1( "������ӿ��£��ü��������һ����Y/N��? ", WhiteColor ); 
				getchoice( ch, 'y', 'n', "ѡ����һ��" );
				if (ch == 'y') {
					steps++;
					continue;
				}
				else {
					PrintScore( );
					PrintMenu_1( );					// ��ӡһ���˵�
					return; 
				}
			}
			else {
				DrawText_1( "��������ӿ��£�����Ҷ���һ����Y/N��? ", WhiteColor ); 
				getchoice( ch, 'y', 'n', "ѡ����һ��" );
				if (ch == 'y') {
					steps++;
					continue;
				}
				else {
					PrintScore( );
					PrintMenu_1( );					// ��ӡһ���˵�
					return; 
				}
			}
		}

		if (now == HUMAN)								// ��������
		{
			if (steps != 0) {
				lstrcpy( StrBuf , "�ֵ������ˣ������������� " ) ;	// StrBuf ��׼��"�����" ������
				Coordinate(comp_x, comp_y, StrBuf2 );		// ����������ַ���
				lstrcat( StrBuf , StrBuf2 ) ; 				// StrBuf ��StrBuf2 ƴ��
				DrawText_2( StrBuf, WhiteColor ); 	
			}

			while( ProcessMessage() == 0 )
			{
				checkMouse( i , j ) ;					// ��ȡ��갴��λ��
				if ( CheckHitKey( KEY_INPUT_ESCAPE ) != 0 ){	// Escape �˳���Ϸ
					DrawText_1( "���ǵ������˵��ϲ��ô����Y/N��", WhiteColor );
					getchoice (ch,'y', 'n', "�˻��ϲ�");
					if (ch == 'y') {
						PrintMenu_1( );					// ��ӡһ���˵�
						return; 
					}
					else continue;
				}
				if ( CheckHitKey( KEY_INPUT_0 ) != 0 )
				{
					help( );							// �����˿����ӵ�λ��
					continue;
				}
				if ( CheckHitKey( KEY_INPUT_1 ) != 0 ) {// ��������
					SaveBoard( );
					continue;
				}
				if ( CheckHitKey( KEY_INPUT_2 ) != 0 ) {// ѡ������
					Level( );
					continue;
				}

				if ( ( i >= 0 ) && ( i < SCALE ) && ( j >= 0 ) && ( j < SCALE ) ) 
				{
					if (!human[i][j]) {
						continue;
					}
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
			DrawText_2( "�԰����꣬������˼��          ", WhiteColor ); 	
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
	DrawText_3( "һ���˵���0(����) 1(����) 2(д��) ESC(�˳�)", WhiteColor); 		
}

void PrintMenu_2( )										// ��ӡ�����˵�
{
	DrawText_3( "�����˵���0(����) 1(д��) 2(����) ESC(�˻�)", WhiteColor); 		
}

void Level( )
{
	char ch;
	DrawText_1( "����(1/2)��1.����(��ת�仯) 2.�߼�(���Ĳ�)", WhiteColor); 		
	getchoice (ch, '1', '2', "ѡ����") ;
	if( ch == '1' ) {
		DEPTH = LOW;
		MarkTime = 500;									// �ͼ��壬�����㹻ʱ�䣬����۲�仯
		// �ͼ��壬��ʾ��ת��ɫ
		HumanMark = GetColor( 0 , 120 , 120 );			// ���ǳ��ɫ���Ǽ�����ձ仯��
		ComputerMark = GetColor( 100 , 20 , 20 );		// ���ǳ��ɫ���Ǽ�����ձ仯��
		MarkPut = RedColor = GetColor( 200 , 20 , 20 );	// ���ڱ�Ǽ�������µ���ɫ����ɫ
	}
	else {
		DEPTH = HIGH;
		MarkTime = 0;
		if ( HUMAN == BLACK) {
			HumanMark = BlackColor;						
			ComputerMark = WhiteColor;					
		}
		else  {
			HumanMark = WhiteColor;						
			ComputerMark = BlackColor;					
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
				Coordinate( i, j, StrBuf2 );
				lstrcat( StrBuf , StrBuf2 ) ; 			// StrBuf ��StrBuf2 ƴ��
				DrawText_2( StrBuf, RedColor ); 		
				
				count++;
				if (count >= 5)
					return;
			}
	if (count == 0)
	{
		lstrcpy( StrBuf , "����޴�������" ) ;			// StrBuf �д洢�˵�
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

	lstrcpy( StrBuf , "     A  B   C   D  E   F   G   H  " );
	DrawString( 0 , 0, StrBuf ,  WhiteColor ) ;
	DrawString( 0, (SCALE+1) * BOARD_SIZE + 10, StrBuf, WhiteColor); 
		
	// ���Աߵ����� 0 - 7
	for( i = 1 ; i <= SCALE ; i ++ ) {
		ch[0] = (i-1) + '0';  ch[1] = '\0';
		lstrcpy( StrBuf , ch);
		lstrcat( StrBuf , "                                  " );
		lstrcat( StrBuf , ch);
		DrawString( 0, i * BOARD_SIZE + 10 , StrBuf, WhiteColor); 
	}
	
	// �����̵�ɫ
	DrawBox( BOARD_SIZE , BOARD_SIZE ,(SCALE+1) * BOARD_SIZE ,  (SCALE+1) * BOARD_SIZE , 
				GreenColor , TRUE ); 
	// ������
	for( i = 2 ; i <= SCALE ; i ++ )
		DrawLine(BOARD_SIZE , i * BOARD_SIZE , (SCALE+1) * BOARD_SIZE,  i * BOARD_SIZE ,  GetColor( 20 , 20 , 20) ) ;
	// ������
	for( j = 2 ; j <= SCALE ; j ++ )
		DrawLine(j * BOARD_SIZE , BOARD_SIZE,  j * BOARD_SIZE , (SCALE+1) * BOARD_SIZE,  GetColor( 20 , 20 , 20) ) ;
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
	size = BOARD_SIZE / CIRCLE_Adj ;		

	DrawCircle( int(( i  + 1.5 ) * BOARD_SIZE ) , int(( j + 1.5 ) * BOARD_SIZE ), 
			size, chesscolor, TRUE ) ;	
}

void MarkChess( int i , int j, char role ) 
{
	int size;
	size = BOARD_SIZE / CIRCLE_Adj;						// CIRCLE_Adj = 3.75 
	if (role == COMPUTER)
		DrawCircle( int(( i  + 1.5 ) * BOARD_SIZE ) , int(( j + 1.5 ) * BOARD_SIZE ), 
			size, ComputerMark, TRUE ) ;	
	else DrawCircle( int(( i  + 1.5 ) * BOARD_SIZE ) , int(( j + 1.5 ) * BOARD_SIZE ), 
			size, HumanMark, TRUE ) ;
	WaitTimer(MarkTime);
}

void ColorChess( int i , int j, int chesscolor ) 
{
	int size;
	size = BOARD_SIZE / CIRCLE_Adj;						// CIRCLE_Adj = 3.75 
	DrawCircle( int(( i  + 1.5 ) * BOARD_SIZE ) , int(( j + 1.5 ) * BOARD_SIZE ), 
			size, chesscolor, TRUE ) ;	
}

void DrawText_1( char StrBuf[], int color ) 
{
	// �����������
	DrawBox( 0 , (SCALE + 2) * BOARD_SIZE, 639 ,  (SCALE + 2) * BOARD_SIZE + 24 , BlackColor , TRUE ) ;
	// ��ӡ
	DrawString( 0 , (SCALE + 2) * BOARD_SIZE + 4 , StrBuf , color ) ;
}

void DrawText_2( char StrBuf[], int color ) 
{
	// �����������
	DrawBox(0 , ( SCALE + 3 ) * BOARD_SIZE, 639 ,  ( SCALE + 3 )  * BOARD_SIZE + 20 , BlackColor , TRUE ) ;
	// ��ӡ
	DrawString( 0 , ( SCALE + 3 ) * BOARD_SIZE + 4 , StrBuf , color ) ;
}

void DrawText_3( char StrBuf[], int color ) 
{
	// �����������
	DrawBox(0 , ( SCALE + 4 ) * BOARD_SIZE, 639 ,  ( SCALE + 4 )  * BOARD_SIZE + 20 , BlackColor , TRUE ) ;
	// ��ӡ
	DrawString( 0 , ( SCALE + 4 ) * BOARD_SIZE + 4 , StrBuf , color ) ;
}

void Coordinate(int i, int j, char StrBuf[ ])
{
	char StrBuf2[ 32 ], ch[2];
	if (i >= 0 && i <= 7 ) 
		ch[0] = i + 'A';		
	else ch[0] = 'X'; 
	ch[1] = '\0';
	lstrcpy( StrBuf , "(" ) ;
	lstrcat( StrBuf , ch ) ;						// StrBuf ��ch ƴ��
	lstrcat( StrBuf , ", " ) ;						// StrBuf ��ch ƴ��
	itoa( j , StrBuf2 , 10 ) ;						// MouseY ֵת������������ִ���StrBuf2
	lstrcat( StrBuf , StrBuf2 ) ; 					// StrBuf ��StrBuf2 ƴ��
	lstrcat( StrBuf , ") " ) ;
}

void checkMouse( int &i , int &j )
{
	char StrBuf[ 128 ] , StrBuf2[ 32 ] ;
	int MouseInput ;
	int MouseX , MouseY ;

	// �������
	while( CheckHitKeyAll() == 0 )
	{
		GetMousePoint( &MouseX , &MouseY ) ;			// ��ȡ���λ��

		i = MouseX / BOARD_SIZE - 1;	
		j = MouseY / BOARD_SIZE - 1;

		lstrcpy( StrBuf , "���λ�ã�" ) ;
		Coordinate( i, j, StrBuf2 );
		lstrcat( StrBuf , StrBuf2 ) ; 					// StrBuf ��StrBuf2 ƴ��

		MouseInput = GetMouseInput() ;
		DrawText_1(StrBuf, WhiteColor); 
		
		// �ж��Ƿ�������ȷ�����룬��������˳����������ֵ
		if ( ( MouseInput & MOUSE_INPUT_LEFT ) == 0 )
			if( ProcessMessage() == -1 )				// �¼�����
				break ;									// �˳�ѭ��
		WaitTimer( 100 ) ;								// �ȴ�һ��ʱ�䣬������Ļ�����ֵ�仯̫��
	}
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
	itoa( countHuman , StrBuf2 , 10 ) ;					// ��ҵ÷�ֵת�ɺ���������ִ���StrBuf2
	lstrcat( StrBuf , StrBuf2 ) ;						// StrBuf ��StrBuf2 ƴ��
	lstrcat( StrBuf , "  �������" ) ;					// StrBuf ��������"������÷�"
	itoa( countComputer , StrBuf2 , 10 ) ;				// ������÷�ֵת������������ִ���StrBuf2
	lstrcat( StrBuf , StrBuf2 ) ; 						// StrBuf ��StrBuf2 ƴ��

	if ( countHuman < countComputer)
		lstrcat( StrBuf , " ���ź������ˣ�");
	else if(countHuman > countComputer )
		lstrcat( StrBuf ,"  ���ģ���������ˣ�\n");
	else
		lstrcat( StrBuf , " ƽ�ְ�~~~�ѷ�ʤ��");
	DrawText_2( StrBuf, RedColor); 			
}

void getchoice(char &ch, char c1, char c2, char str[ ])
{
	char StrBuf[ 128 ] ;
	char before[60],o_b;
	int l_b,i;

	while(1)
	{	
		lstrcpy( StrBuf , "                                       ");	// ��մ��������Ļ����
		DrawText_2( StrBuf, WhiteColor); 
		KeyInputString( 0 , ( SCALE + 3 ) * BOARD_SIZE, 639 , before , FALSE ) ;	
		while(before[0]=='\0')
			KeyInputString( 0 , ( SCALE + 3 ) * BOARD_SIZE, 639 , before , FALSE ) ;	
		l_b = strlen(before);
		o_b = before[0];
		for (i=0;i<l_b-1;i++)
		{
			if (before[i]==' ')
				o_b = before[i+1];
			else
			break;
		}
        if(isupper(o_b)) 
			o_b = tolower(o_b); 
		if((o_b == c1) || (o_b == c2))
			break;
		else
		{
			StrBuf[0] = c1; StrBuf[1] = '/'; StrBuf[2] = c2; StrBuf[3] = '\0';   
			lstrcat( StrBuf , str ) ;									// �����Ĺ�������	
			lstrcat( StrBuf , " ��������ȷ��ѡ��                 " ) ;  // StrBuf �д洢�˵�	
			DrawText_1( StrBuf, WhiteColor );
		}
	}
	ch = o_b;
}

void choose_Color( )
{
	char ch;

	DrawText_1( "ѡ��(1/2 )��1.���ӣ����֣� 2.���ӣ����֣�", WhiteColor);
	
	while( !ProcessMessage() && ( CheckHitKey( KEY_INPUT_ESCAPE ) == 0 ) )
	{
		getchoice (ch, '1', '2', "ѡɫ") ;

		if( ch == '1' )
		{
			HUMAN=BLACK;
			COMPUTER=WHITE;

			possible();
			DrawText_2( "�������֣���������                  ", WhiteColor); 
			DrawBoard();
			break;									
		}
		else if ( ch == '2' ) {
			HUMAN=WHITE;
			COMPUTER=BLACK;
			possible();
			DrawText_2(  "���Ӻ��֣����ɸ���ǫ�ã����²�������", WhiteColor); 

			srand((unsigned)time(NULL));			// ��ʼ��
			int a=rand();							// �����
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
			ColorChess( can[a%num][0],can[a%num][1], MarkPut );
			change_color(can[a%num][0],can[a%num][1],COMPUTER,board, true);
			possible( );
			break;
		}
	}
}

void SaveBoard()										// ���ļ���д��������
{
	int i, j, k = 0;
	char StrBuf[ 128 ] ;								// ��ʾ��Ϣ�õĻ���

	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;  // �Ǻǡ�����ô���ĳ��򶼱�����ˣ����һ��������������鳤��������ջ�������
	bool ExistOK = true;

	ch = '0';
	DrawText_1(  "д�̡��������ļ�����<30���� " , WhiteColor );
	DrawText_2( "                                             ", WhiteColor); 

	KeyInputString( 0 , ( SCALE + 3 ) * BOARD_SIZE, 639 , filename , FALSE ); // gets(filename);
	if (access(filename, 0) == 0)   //�Ѿ�����
	{
		lstrcpy( StrBuf , filename ) ;		
		lstrcat( StrBuf , "�ļ��Ѿ������ˣ��ܸ������𣿣�Y/N��" );
		DrawText_1( StrBuf, WhiteColor);

		while( !ProcessMessage() && ( CheckHitKey( KEY_INPUT_ESCAPE ) == 0 ) )
		{
			getchoice (ch, 'y', 'n', "�����ļ�");
			if ( ch == 'y' )
			{
				break;									// �Ǿ�����������ˣ������ء����˼Ҷ�˵�����ˣ�
			}
			else if ( ch == 'n' ) {
				ExistOK = false;
				break;
			}
		}
	}
	if (!ExistOK)
		return;

	err  = fopen_s( &fp, filename, "w+" );

	if( err != 0 )
	{
		lstrcpy( StrBuf , filename ) ;	
		lstrcat( StrBuf , "�ļ���ʧ��           " );
		DrawText_2( StrBuf, WhiteColor ); 
		return;
	}
	else {
		lstrcpy( StrBuf , filename ) ;
		lstrcat( StrBuf , "�ļ��ɹ����棬�������Ϸ" );
		DrawText_2( StrBuf, WhiteColor ); 
	}

	for(i=0;i<SCALE;i++)								// ����
	{
		for(j=0;j<SCALE;j++) 
		{
			str[k++] = board[i][j];
		}
		str[k++]='\n';									// �ӻس�
	}
	str[k++]=COMPUTER; 
	str[k++]=HUMAN;

    fwrite(str,1,(SCALE+1)*SCALE+2,fp);	// ����ԭ��size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
										// ע�⣬���� str��û�б���"\0"�ַ�����������������    fwrite(str,1,strlen(str),fp);	
    fclose(fp);											// �ǵù��ϡ���
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
			{
				temp[i][j]=1;
			}
			else
				if(a[i][j]==EMPTY)
			    {
				    temp[i][j]=0;
			    }
				else									// ���ڷǷ��ַ�
				{
					return -1;
				}
		}
	}
	for(i=SCALE/2-1;i<=SCALE/2;i++)
	{
		for(j=SCALE/2-1;j<=SCALE/2;j++)
		{
			if(!temp[i][j])								// �������û�����ӣ�һ���Ƿ�
				return 0;
		}
	}
	walk(SCALE/2,SCALE/2,temp);
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(temp[i][j])
				return 0;								// δ����һƬ��һ������
		}
	}
	return 1;
}

bool ReadBoard()										// ���ļ��ж�����������
{
	int i, j, k = 0;
	char StrBuf[ 128 ] ;								// ��ʾ��Ϣ�õĻ���

	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;  // �Ǻǡ�����ô���ĳ��򶼱�����ˣ����һ��������������鳤��������ջ�������
	int okay;

	ch='0';

	DrawText_1( "���̡��������ļ�����<30���� ", WhiteColor );
	DrawText_2( "                                              ", WhiteColor); 

	KeyInputString( 0 , ( SCALE + 3) * BOARD_SIZE, 639 , filename , FALSE ); // gets(filename);
	err  = fopen_s( &fp, filename, "r" );
	if( err != 0 ) {
		lstrcpy( StrBuf , filename ) ;	
		lstrcat( StrBuf , "�ļ���ʧ��" );
		DrawText_2( StrBuf, WhiteColor ); 
		return false;
	}

	fread(str,1,SCALE*(SCALE+1)+2,fp);	
	// ����ԭ��size_t fread(void *buffer, size_t size, size_t count, FILE *stream);��

	for (i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++) 
		{
			save_board[i][j] = str[k++];
		}
		k++;											// ȥ���س�
	}
	char computer=str[k++];
	char human=str[k++];

    fclose(fp);											// �ù��͹غ�ϰ��

	okay = board_check(save_board);

	if(okay == 1)
	{
		for(i=0;i<SCALE;i++)
		{
			for(j=0;j<SCALE;j++)
			{
				board[i][j] = save_board[i][j];
			}
		}
		HUMAN=human;
		COMPUTER=computer;
		DrawBackground( );								// �����̶��ػ�һ��
		DrawBoard( );
		lstrcpy( StrBuf , filename ) ;
		lstrcat( StrBuf , "�ɹ����룬��ѡ����ɫ" );
		DrawText_2( StrBuf, WhiteColor ); 
		return true;
	}
	else {
		DrawText_2( "��Ǹ�������ļ����Դ���������ѡ��", WhiteColor); 
		return false;
	}
}

void put_value()
{
	int i,j;
	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
			const_value[i][j]=1;
	const_value[0][0]=const_value[0][SCALE-1]
	=const_value[SCALE-1][0]=const_value[SCALE-1][SCALE-1]=800;
	for(i=1;i<SCALE-1;i++)
	{
		const_value[0][i]=100;
		const_value[SCALE-1][i]=100;
		const_value[i][0]=100;
		const_value[i][SCALE-1]=100;
	}
	const_value[0][1]=const_value[1][0]=const_value[1][1]=-200;
	const_value[0][SCALE-2]=const_value[1][SCALE-2]=const_value[1][SCALE-1]=-200;
	const_value[SCALE-1][1]=const_value[SCALE-2][0]=const_value[SCALE-2][1]=-200;
	const_value[SCALE-1][SCALE-2]=const_value[SCALE-2][SCALE-1]=const_value[SCALE-2][SCALE-2]=-200;

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
	{
		for(j=0;j<SCALE;j++)
		{
			human[i][j]=computer[i][j]=0;
			if(judge_put(i,j,HUMAN,board))
				human[i][j]=1;
			if(judge_put(i,j,COMPUTER,board))
				computer[i][j]=1;
		}
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

void c_val(char board[SCALE][SCALE])
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
	    change_value[0][1]=change_value[1][0]=change_value[1][1]=-300;
	if(board[0][SCALE-1]==HUMAN)
	    change_value[0][SCALE-2]=change_value[1][SCALE-2]=change_value[1][SCALE-1]=-300;
	if(board[SCALE-1][0]==HUMAN)
	    change_value[SCALE-1][1]=change_value[SCALE-2][0]=change_value[SCALE-2][1]=-300;
	if(board[SCALE-1][SCALE-1]==HUMAN)
	    change_value[SCALE-1][SCALE-2]=change_value[SCALE-2][SCALE-1]=change_value[SCALE-2][SCALE-2]=-300;
	// �Է�ռ�ߵĲ��Ա仯�ִ���
	for(i=1;i<SCALE-1;i++)
	{
		if(board[0][i]==HUMAN)
		{
			change_value[0][i-1]-=100;
			change_value[0][i+1]-=100;
		}
		if(board[SCALE-1][i]==HUMAN)
		{
			change_value[SCALE-1][i-1]-=100;
			change_value[SCALE-1][i+1]-=100;
		}
		if(board[i][0]==HUMAN)
		{
			change_value[i-1][0]=-100;
			change_value[i+1][0]=-100;
		}
		if(board[i][SCALE-1]==HUMAN)
		{
			change_value[i-1][SCALE-1]-=100;
			change_value[i+1][SCALE-1]-=100;
		}
	}
}

//��ֵ����
//����ǰ��ֹ�ֵ

int temp_value(char board[SCALE][SCALE])
{
	c_val(board);
	int score=0;
	int i,j;
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(board[i][j]==COMPUTER)  
			{
				score+=steady(i,j,COMPUTER,board)*30;	// �ȶ���
				score+=(const_value[i][j]+change_value[i][j]);  // λ�ù���Ȩ��+�ɱ�Ȩ��
			}
			else
				if(board[i][j]==HUMAN)
				{
					score-=steady(i,j,HUMAN,board)*30;
					score-=(const_value[i][j]-change_value[i][j]);  //ע��change_valueȫ���Լ�����Ƕ�Ϊ׼�ģ�
				}
			if(judge_put(i,j,COMPUTER,board))			// �ж���
				score+=100;
			if(judge_put(i,j,HUMAN,board))
				score-=100;
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
					if(a==0)							// ֻ��Ϊ��ȷ���н�
														// ��Ȼ����Ҳ�����˵�ǲ���ȫ�Ħ��¼�֦
														// ��Ϊ�ҵ�min - max����Ҳ���Ǻ���ͳ��
				    {
					    alpha=temp;						// �ð���û˵�滰���滰���ҵ����������ÿ����ܳ������ͷ�Χ�ġ�
						temp_x=i;
					    temp_y=j;
					    a++;
					    continue;  
				    }
				    if(alpha<temp)
				    {
					    alpha=temp;
						temp_x=i;
						temp_y=j;
				    }
					if(alpha>=beta)
					{
						if(depth==0)
						{
							comp_x=temp_x;
							comp_y=temp_y;				// �����������Ǵ������ﰡ��������ָ���˲��𡭡�
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