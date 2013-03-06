#ifndef __UTILS_H__
#define __UTILS_H__

#include <msp430x14x.h>
#include <msp430.h>
#include <legacymsp430.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define bitset(var,bitno) ((var) |= 1 << (bitno))
#define bitclr(var,bitno) ((var) &= ~(1 << (bitno)))

#define          B1                 BIT4&P4IN	//B1 - P4.4
#define          B2                 BIT5&P4IN	//B2 - P4.5
#define          B3                 BIT6&P4IN	//B3 - P4.6
#define          B4                 BIT7&P4IN	//B4 - P4.7
#define          LCD_Data           P2OUT
#define          _100us             66	//66 cycles *12 + 9 = 801 / 801*125ns = 100us
#define          _10us              6	//6 cycles * 12 + 9 = 81 / 81*125ns=10us
#define          E                  3	//P2.3
#define          RS                 2	//P2.2
#define          CR                 0x0d
#define          LF                 0x0a
#define          BUTTON_TIME        100

#define		DISP_ON			0x0c	//LCD control constants
#define		DISP_OFF		0x08	//
#define		CLR_DISP		0x01	//
#define		CUR_HOME		0x02	//
#define		ENTRY_INC		0x06	//
#define		DD_RAM_ADDR		0x80	//
#define		DD_RAM_ADDR2		0xc0	//
#define		DD_RAM_ADDR3		0x28	//
#define		CG_RAM_ADDR		0x40	//

void Delay(unsigned int a);
void Delayx100us(unsigned char b);
void SEND_CHAR(unsigned char c);
void SEND_CMD(unsigned char e);
void InitLCD(void);

#endif
