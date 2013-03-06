#include "utils.h"

unsigned char RXData, i, j, k, temp, cntr, time_out;


void Delay(unsigned int a)
{
	for (k = 0; k != a; ++k) ;	//9+a*12 cycles
}

void Delayx100us(unsigned char b)
{
	for (j = 0; j != b; ++j)
		Delay(_100us);
}

void _E(void)
{
	bitset(P2OUT, E);	//toggle E for LCD
	Delay(_10us);
	bitclr(P2OUT, E);
}

void SEND_CHAR(unsigned char d)
{
	Delayx100us(5);		//.5ms  
	temp = d & 0xf0;	//get upper nibble      
	LCD_Data &= 0x0f;
	LCD_Data |= temp;
	bitset(P2OUT, RS);	//set LCD to data mode
	_E();			//toggle E for LCD
	temp = d & 0x0f;
	temp = temp << 4;	//get down nibble
	LCD_Data &= 0x0f;
	LCD_Data |= temp;
	bitset(P2OUT, RS);	//set LCD to data mode
	_E();			//toggle E for LCD
}

void SEND_CMD(unsigned char e)
{
	Delayx100us(10);	//10ms
	temp = e & 0xf0;	//get upper nibble      
	LCD_Data &= 0x0f;
	LCD_Data |= temp;	//send CMD to LCD
	bitclr(P2OUT, RS);	//set LCD to CMD mode
	_E();			//toggle E for LCD
	temp = e & 0x0f;
	temp = temp << 4;	//get down nibble
	LCD_Data &= 0x0f;
	LCD_Data |= temp;
	bitclr(P2OUT, RS);	//set LCD to CMD mode
	_E();			//toggle E for LCD
}

void InitLCD(void)
{
	bitclr(P2OUT, RS);
	Delayx100us(250);	//Delay 100ms
	Delayx100us(250);
	Delayx100us(250);
	Delayx100us(250);
	LCD_Data |= BIT4 | BIT5;	//D7-D4 = 0011
	LCD_Data &= ~BIT6 & ~BIT7;
	_E();			//toggle E for LCD
	Delayx100us(100);	//10ms
	_E();			//toggle E for LCD
	Delayx100us(100);	//10ms
	_E();			//toggle E for LCD
	Delayx100us(100);	//10ms
	LCD_Data &= ~BIT4;
	_E();			//toggle E for LCD

	SEND_CMD(DISP_ON);
	SEND_CMD(CLR_DISP);
}

