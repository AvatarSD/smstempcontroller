/*
 * lcdterminal.cpp
 *
 *  Created on: 2 лист. 2015
 *      Author: sd
 */

#include "lcdterminal.h"


#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <util/atomic.h>
#include <util/delay.h>



LCDterm debug;

LCDterm::LCDterm()
{
	memset(videoBuff, ' ', LCD_DISP_LENGTH*LCD_LINES);
	videoBuff[LCD_DISP_LENGTH*LCD_LINES] = '\0';
	pBuff = 0;
}

LCDterm::~LCDterm()
{

}

void LCDterm::putch(char c)
{
	static char lastChar = 0;
	static bool lastLF = false;
	/*
	if((lastLF == true)&&(lastChar != '\n')&&(c == '\n'))
	{
		lastChar = c;
		lastLF = false;
		return;
	}
	 */

	lastChar = c;
	lastLF = shiftBuff();;

	if(c == '\n')
		//		if((!lf)||(lastChar == '\n'))
		pBuff += (LCD_DISP_LENGTH-(pBuff%LCD_DISP_LENGTH));


	/*if(c == '\r')
	pBuff = (pBuff/LCD_DISP_LENGTH)*LCD_DISP_LENGTH;
	 */

	if(c < 0x20)
		return;

	videoBuff[pBuff++] = c;


}

bool LCDterm::shiftBuff()
{
	if(pBuff == LCD_DISP_LENGTH)
		pBuff = LCD_DISP_LENGTH*2;
	else if(pBuff == LCD_DISP_LENGTH*3)
		pBuff = LCD_DISP_LENGTH;
	else if(pBuff == LCD_DISP_LENGTH*2)
		pBuff = LCD_DISP_LENGTH*3;
	else if(pBuff == LCD_DISP_LENGTH*4)
	{
		pBuff = LCD_DISP_LENGTH*3;
		memcpy(videoBuff,
				videoBuff+LCD_DISP_LENGTH*2,
				LCD_DISP_LENGTH);
		memcpy(videoBuff+LCD_DISP_LENGTH*2,
				videoBuff+LCD_DISP_LENGTH,
				LCD_DISP_LENGTH);
		memcpy(videoBuff+LCD_DISP_LENGTH,
				videoBuff+LCD_DISP_LENGTH*3,
				LCD_DISP_LENGTH);
		memset(videoBuff+LCD_DISP_LENGTH*3, ' ',
				LCD_DISP_LENGTH);
	}
	else
		return false;
	return true;
}
void LCDterm::refreshScreen()
{
	lcd_home();
	lcd_puts(videoBuff);
}

void LCDterm::print(const char * str)
{
	int curr = 0;
	while(str[curr] != '\0')
		putch(str[curr++]);
	refreshScreen();
}

void LCDterm::print(long int num)
{
	char buf[12];
	sprintf(buf, "%ld", num);
	print(buf);
}

void LCDterm::print(int num)
{
	char buf[12];
	sprintf(buf, "%d", num);
	print(buf);
}

void LCDterm::print(unsigned long int num)
{
	char buf[12];
	sprintf(buf, "%lu", num);
	print(buf);
}

void LCDterm::print(unsigned int num)
{
	char buf[12];
	sprintf(buf, "%u", num);
	print(buf);
}

void LCDterm::print(double num)
{
	char buf[12];
	sprintf(buf, "%f", num);
	print(buf);
}

void LCDterm::print(const __FlashStringHelper * flashStr)
{
	char readedByte, *flashPointer = (char*)flashStr;
	readedByte = pgm_read_byte(flashPointer);
	while(readedByte)
	{
		putch(readedByte);
		flashPointer++;
		readedByte = pgm_read_byte(flashPointer);
	}
	refreshScreen();
}



