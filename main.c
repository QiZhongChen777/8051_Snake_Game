#include <AT89X51.h>
#include <stdlib.h>
#include "Button_Function.h"
#define Key_one 0x01					
#define Key_two 0x02
#define Key_three 0x03
#define Key_four 0x04
#define MATRIX_LED_PORT		P2
sbit RCK=P0^1;		//RCLK
sbit SCK=P0^2;		//SRCLK
sbit SER=P0^0;		//SER
unsigned char KeyCode = 0;
unsigned char KeyRam = 0;				
unsigned char PreKeyRam = 0;
unsigned char KeyCnt = 0;	
unsigned char NoKeyCnt = 0;			
unsigned char Key_Value = 0;
unsigned char T30ms_cnt = 3;

bit turn_up = 0,turn_down = 0,turn_right = 0,turn_left = 0;
bit game_up = 0;

unsigned char len = 2;
char x = 0,y = 0;
unsigned char index = 0;
unsigned char cur_tail = 0;

unsigned char Snake[8][8]={0};
char Snake_food[1][2]={-1};
char Snake_tail[8][2]={-1};

void _74HC595_WriteByte(unsigned char Byte);
void LED_Matrix(unsigned char Column,unsigned char Row_Shift,Data);
void fixEdge();
void Key_Function();
void Snake_Move();
void delay_1ms(int n);
void GenerateFood();
unsigned char isSnakeCell(unsigned char row, unsigned char col);
void Snake_init();


void main() 
{
	unsigned char i;	
	turn_up = 0,turn_down = 0,turn_right = 0,turn_left = 0;
	SCK=0;
	RCK=0;
	TMOD = 0x01;
	EA = 1;
	TL0 = (65536 - 10000) % 256;
	TH0 = (65536 - 10000) / 256;
	TR0 = 1;
	ET0 = 1;

	for(i=0;i<100;i++)
	{
		LED_Matrix(0,0,0x00);
		LED_Matrix(1,0,0x0E);
		LED_Matrix(2,0,0x06);
		LED_Matrix(3,0,0x0A);
		LED_Matrix(4,0,0x10);
		LED_Matrix(5,0,0x20);
		LED_Matrix(6,0,0x40);
		LED_Matrix(7,0,0x80);
	}
	Snake_init();
	
	
	while(1)
	{
		if(game_up)
		{
			GenerateFood();
			Key_Function();	
			
			for(i=0;i<35-3*len;i++)
			{
				Snake_Move();
			}		
		}
		else
		{
			for(i=0;i<100;i++)
			{
				LED_Matrix(0,0,0x10);
				LED_Matrix(1,0,0x20);
				LED_Matrix(2,0,0x12);
				LED_Matrix(3,0,0x02);
				LED_Matrix(4,0,0x02);
				LED_Matrix(5,0,0x12);
				LED_Matrix(6,0,0x20);
				LED_Matrix(7,0,0x10);
			}
			for(i=0;i<100;i++)
			{
				LED_Matrix(0,0,0x20);
				LED_Matrix(1,0,0x38);
				LED_Matrix(2,0,0x22);
				LED_Matrix(3,0,0x02);
				LED_Matrix(4,0,0x02);
				LED_Matrix(5,0,0x22);
				LED_Matrix(6,0,0x38);
				LED_Matrix(7,0,0x20);
			}
		}
	}
}

void timer0_isr() interrupt 1												
{
	TL0 = (65536 - 10000) % 256;
	TH0 = (65536 - 10000) / 256;
	if(--T30ms_cnt == 0 )
	{	
		T30ms_cnt = 3;
		ReadKey(&KeyRam);
		Key_Value = GetKey(&KeyCode,&KeyRam,&PreKeyRam,&KeyCnt,&NoKeyCnt);		
	}
}

void _74HC595_WriteByte(unsigned char Byte)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		SER=Byte&(0x80>>i);
		SCK=1;
		SCK=0;
	}
	RCK=1;
	RCK=0;
}


void LED_Matrix(unsigned char Column,unsigned char Row_Shift,Data)
{
	_74HC595_WriteByte(Data<<Row_Shift);
	MATRIX_LED_PORT = ~(0x80>>Column);
	delay_1ms(1);
	MATRIX_LED_PORT = 0xFF;
}


void delay_1ms(int n)
{
	unsigned int i,j;
	for (i = 0; i < n; i++)
	for (j = 0; j < 120; j++) ;
}

void Key_Function()
{
	if((Key_Value) != 0)
	{		
		switch((Key_Value))
		{
			case Key_one:
				
				if(turn_down != 1)
				{
					y++;
					index++;
					fixEdge();

				
					turn_up = 1;
					turn_down = 0,turn_right = 0,turn_left = 0;
							
					if(Snake[x][y] == 1 && x != Snake_food[0][0] && y != Snake_food[1][0])
					{
						game_up = 0;
						break;
					}
	
					Snake[x][y] = 1;
					Snake_tail[index][0] = x;
					Snake_tail[index][1] = y;
					
					Snake[Snake_tail[cur_tail][0]][Snake_tail[cur_tail][1]] = 0;
					cur_tail++;

				}
				(Key_Value) = 0;
//				(KeyCode) = 0;
				break;		
								
			case Key_two:
				
				if(turn_up != 1)
				{
					y--;
					index++;
					fixEdge();			
				
					turn_down = 1;
					turn_up = 0,turn_right = 0,turn_left = 0;
					
					if(Snake[x][y] == 1 && x != Snake_food[0][0] && y != Snake_food[1][0])
					{
						game_up = 0;
						break;
					}
					
					Snake[x][y] = 1;
					Snake_tail[index][0] = x;
					Snake_tail[index][1] = y;
					
					Snake[Snake_tail[cur_tail][0]][Snake_tail[cur_tail][1]] = 0;
					cur_tail++;
					
					
				}
				(Key_Value) = 0;
//				(KeyCode) = 0;
				break;
				
			case Key_three:
				
				if(turn_right != 1)
				{
					x--;
					index++;
					fixEdge();
					
					turn_left = 1;
					turn_up = 0,turn_down = 0,turn_right = 0;
					
					if(Snake[x][y] == 1 && x != Snake_food[0][0] && y != Snake_food[1][0])
					{
						game_up = 0;
						break;
					}
					
					Snake[x][y] = 1;
					Snake_tail[index][0] = x;
					Snake_tail[index][1] = y;

					Snake[Snake_tail[cur_tail][0]][Snake_tail[cur_tail][1]] = 0;
					cur_tail++;

				}
				(Key_Value) = 0;
//				(KeyCode) = 0;
				break;
				
			case Key_four:
				
				if(turn_left != 1)
				{
					x++;
					index++;
					fixEdge();
				
					turn_right = 1;
					turn_up = 0,turn_down = 0,turn_left = 0;
					
					if(Snake[x][y] == 1 && x != Snake_food[0][0] && y != Snake_food[1][0])
					{
						game_up = 0;
						break;
					}

					Snake[x][y] = 1;
					Snake_tail[index][0] = x;
					Snake_tail[index][1] = y;

					Snake[Snake_tail[cur_tail][0]][Snake_tail[cur_tail][1]] = 0;
					cur_tail++;

				}
				(Key_Value) = 0;
//				(KeyCode) = 0;
				break;

			
			default:
				
				(Key_Value) = 0;
				(KeyCode) = 0;
				break;
		}
	}
}

void fixEdge() 
{
// 	if(x < 0 || x > 7 || y < 0 ||  y > 7)
// 		game_up = 0;
  x < 0 ? x += 8 : 0;
  x > 7 ? x -= 8 : 0;
  y < 0 ? y += 8 : 0;
  y > 7 ? y -= 8 : 0;
	
	index > 7 ? index -= 8 : 0;
	cur_tail > 7 ? cur_tail -= 8 : 0;
}


void Snake_Move() 
{
	unsigned char i,j;
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			if( Snake[i][j] == 1 )
			{
				LED_Matrix(i,j,Snake[i][j]);
			}
		}
	}
}


void GenerateFood() 
{
	unsigned char row, col;
	if(Snake_food[0][0] == -1 || Snake[Snake_food[0][0]][Snake_food[0][1]] == 0)
	{
		if(Snake[Snake_food[0][0]][Snake_food[0][1]] == 0 && len < 7 )
		{
			if(turn_up == 1)
			{
				y++;
				index++;
				fixEdge();					
				Snake[x][y] = 1;
				Snake_tail[index][0] = x;
				Snake_tail[index][1] = y;				
			}
			else if(turn_down == 1)
			{
				y--;
				index++;
				fixEdge();					
				Snake[x][y] = 1;
				Snake_tail[index][0] = x;
				Snake_tail[index][1] = y;	
				
			}
			else if(turn_left == 1)
			{
				x--;
				index++;
				fixEdge();					
				Snake[x][y] = 1;
				Snake_tail[index][0] = x;
				Snake_tail[index][1] = y;	
				
			}
			else if(turn_right == 1)
			{
				x++;
				index++;
				fixEdge();					
				Snake[x][y] = 1;
				Snake_tail[index][0] = x;
				Snake_tail[index][1] = y;	
				
			}
			len++;
		}
		
		do
		{
			row = rand() % 8;
			col = rand() % 8;
		}while(isSnakeCell(row,col));

		Snake_food[0][0]=row;
		Snake_food[0][1]=col;
		Snake[Snake_food[0][0]][Snake_food[0][1]] = 1;
	}
}

unsigned char isSnakeCell(unsigned char row, unsigned char col) 
{
	if(Snake[row][col])
		return 1;
	return 0;
}



void Snake_init()
{
	unsigned char i,j;
	for(i=0;i<len;i++)
	{
		x = i;
		index = i;
		Snake[i][0] = 1;
		Snake_tail[index][0] = i;
		Snake_tail[index][1] = 0;

		for(j=0;j<100;j++)
			Snake_Move();
	}
	turn_right = 1;
	game_up = 1;
}

