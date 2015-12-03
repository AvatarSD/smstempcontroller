/*
 * debug.h
 *
 *  Created on: 21 вер. 2015 р.
 *      Author: sd
 */

#ifndef LOG_DEBUG_H_
#define LOG_DEBUG_H_

#include "../UART/UART.h"
extern UART debugPort;
#include "../LiquidCrystal_I2C/lcdTerminal/lcdterminal.h"
#include "SDCardLogger.h"
#include "../config.h"


#define DEBUG_LEVEL_QUITE 0
#define DEBUG_LEVEL_CRITICAL 1
#define DEBUG_LEVEL_WARNING 2
#define DEBUG_LEVEL_INFO 3
#define DEBUG_LEVEL_DEBUG 4
#define DEBUG_LEVEL_DATA 5


#define debug(x) {debugLCD(x);debugPort(x);debugSDcardLog(x);}


template<class T>
void CRITICAL(T msg)
{
#if (DEBUG_LEVEL_PORT>=DEBUG_LEVEL_CRITICAL)
	debugPort(F("[CRITICAL]: "));
	debugPort(msg);
	debugPort(F("!!!\r\n"));
#endif
#if (DEBUG_LEVEL_LCD>=DEBUG_LEVEL_CRITICAL)
	debugLCD(F("[CRITICAL]: "));
	debugLCD(msg);
	debugLCD(F("!!!\r\n"));
#endif
#if (DEBUG_LEVEL_SDCARD>=DEBUG_LEVEL_CRITICAL)
	debugSDcardLog("[");
	debugSDcardLog(getDateTimeStr());
	debugSDcardLog("]");
	debugSDcardLog(F("[CRITICAL]: "));
	debugSDcardLog(msg);
	debugSDcardLog(F("!!!\r\n"));
#endif
}

template<class T>
void WARNING(T msg)
{
#if (DEBUG_LEVEL_PORT>=DEBUG_LEVEL_WARNING)
	debugPort(F("[WARNING]: "));
	debugPort(msg);
	debugPort(F("\r\n"));
#endif
#if (DEBUG_LEVEL_LCD>=DEBUG_LEVEL_WARNING)
	debugLCD(F("[WARNING]: "));
	debugLCD(msg);
	debugLCD(F("\r\n"));
#endif
#if (DEBUG_LEVEL_SDCARD>=DEBUG_LEVEL_WARNING)
	debugSDcardLog("[");
	debugSDcardLog(getDateTimeStr());
	debugSDcardLog("]");
	debugSDcardLog(F("[WARNING]: "));
	debugSDcardLog(msg);
	debugSDcardLog(F("\r\n"));
#endif
}

template<class T>
void INFO(T msg)
{
#if (DEBUG_LEVEL_PORT>=DEBUG_LEVEL_INFO)
	debugPort(F("[INFO]: "));
	debugPort(msg);
	debugPort(F("\r\n"));
#endif
#if (DEBUG_LEVEL_LCD>=DEBUG_LEVEL_INFO)
	debugLCD(F("[INFO]: "));
	debugLCD(msg);
	debugLCD(F("\r\n"));
#endif
#if (DEBUG_LEVEL_SDCARD>=DEBUG_LEVEL_INFO)
	debugSDcardLog("[");
	debugSDcardLog(getDateTimeStr());
	debugSDcardLog("]");
	debugSDcardLog(F("[INFO]: "));
	debugSDcardLog(msg);
	debugSDcardLog(F("\r\n"));
#endif
}

template<class T>
void DEBUG(T msg)
{
#if (DEBUG_LEVEL_PORT>=DEBUG_LEVEL_DEBUG)
	debugPort(F("[DEBUG]: "));
	debugPort(msg);
	debugPort(F("\r\n"));
#endif
#if (DEBUG_LEVEL_LCD>=DEBUG_LEVEL_DEBUG)
	debugLCD(F("[DEBUG]: "));
	debugLCD(msg);
	debugLCD(F("\r\n"));
#endif
#if (DEBUG_LEVEL_SDCARD>=DEBUG_LEVEL_DEBUG)
	debugSDcardLog("[");
	debugSDcardLog(getDateTimeStr());
	debugSDcardLog("]");
	debugSDcardLog(F("[DEBUG]: "));
	debugSDcardLog(msg);
	debugSDcardLog(F("\r\n"));
#endif
}

template<class T>
void DATA(T msg)
{
#if (DEBUG_LEVEL_PORT>=DEBUG_LEVEL_DATA)
	debugPort(F("[DATA]: "));
	debugPort(msg);
	debugPort(F("\r\n"));
#endif
#if (DEBUG_LEVEL_LCD>=DEBUG_LEVEL_DATA)
	debugLCD(F("[DATA]: "));
	debugLCD(msg);
	debugLCD(F("\r\n"));
#endif
#if (DEBUG_LEVEL_SDCARD>=DEBUG_LEVEL_DATA)
	debugSDcardLog("[");
	debugSDcardLog(getDateTimeStr());
	debugSDcardLog("]");
	debugSDcardLog(F("[DATA]: "));
	debugSDcardLog(msg);
	debugSDcardLog(F("\r\n"));
#endif
}

#endif /* LOG_DEBUG_H_ */
