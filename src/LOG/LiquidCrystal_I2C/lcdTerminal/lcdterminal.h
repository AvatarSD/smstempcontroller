/*
 * lcdterminal.h
 *
 *  Created on: 2 лист. 2015
 *      Author: sd
 */

#ifndef LIQUIDCRYSTAL_I2C_LCDTERMINAL_LCDTERMINAL_H_
#define LIQUIDCRYSTAL_I2C_LCDTERMINAL_LCDTERMINAL_H_

#include "../lcdpcf8574/lcdpcf8574.h"
#include "../../../stream/Stream.h"

class LCDterm : public Stream
{
public:
	LCDterm();
	~LCDterm();

	void refreshScreen();

	void putch(char c);
	int  write(const unsigned char *outbuf, unsigned int outlen);
	bool postSendHandler();

private:
	bool shiftBuff();
	char videoBuff[LCD_DISP_LENGTH*LCD_LINES+1];
	unsigned char pBuff;
};

extern LCDterm debugLCD;


#endif /* LIQUIDCRYSTAL_I2C_LCDTERMINAL_LCDTERMINAL_H_ */
