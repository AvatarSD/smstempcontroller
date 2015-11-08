/*
 * lcdterminal.h
 *
 *  Created on: 2 лист. 2015
 *      Author: sd
 */

#ifndef LIQUIDCRYSTAL_I2C_LCDTERMINAL_LCDTERMINAL_H_
#define LIQUIDCRYSTAL_I2C_LCDTERMINAL_LCDTERMINAL_H_

#include "../lcdpcf8574/lcdpcf8574.h"

class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))

class LCDterm
{
public:
	LCDterm();
	~LCDterm();

	void putch(char c);
	void refreshScreen();

	void print(const char * str);
	void print(const __FlashStringHelper * flashStr);
	void print(long int num);
	void print(int num);
	void print(unsigned long int num);
	void print(unsigned int num);
	void print(double num);
	template<class T>void (* operator()(T str)){print(str);return 0;}

private:
	bool shiftBuff();
	char videoBuff[LCD_DISP_LENGTH*LCD_LINES+1];
	unsigned char pBuff;
};

extern LCDterm debug;

#endif /* LIQUIDCRYSTAL_I2C_LCDTERMINAL_LCDTERMINAL_H_ */
