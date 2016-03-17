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



LCDterm debugLCD;

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

	/*if((pBuff%LCD_DISP_LENGTH == 0)&&(lastChar != '\n')&&(c == '\n'))
	{
		lastChar = c;
		return;
	}*/


	if((pBuff%LCD_DISP_LENGTH == 0)&&(lastChar != ' ')&&(c == ' '))
	{
		lastChar = c;
		return;
	}

	lastChar = c;
	shiftBuff();

	if(c == '\n')
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

int LCDterm::write(const unsigned char *outbuf, unsigned int outlen)
{
	for(unsigned int i = 0; i < outlen; i++)
		putch(outbuf[i]);
	return outlen;
}

bool LCDterm::postSendHandler()
{
	refreshScreen();
	return true;
}


