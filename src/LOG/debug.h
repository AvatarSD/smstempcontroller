/*
 * debug.h
 *
 *  Created on: 21 вер. 2015 р.
 *      Author: sd
 */

#ifndef LOG_DEBUG_H_
#define LOG_DEBUG_H_


#include "../UART/UART.h"


#include "../LiquidCrystal_I2C/lcdTerminal/lcdterminal.h"
extern UART debugPort;

#define debug(x) {debugLCD(x);debugPort(x);}


#define DEBUG_LEVEL_QUITE 0
#define DEBUG_LEVEL_CRITICAL 1
#define DEBUG_LEVEL_WARNING 2
#define DEBUG_LEVEL_INFO 3
#define DEBUG_LEVEL_DEBUG 4
#define DEBUG_LEVEL_DATA 5



/************ Debug Settings & Level ************/
#define DEBUG_TXINT USART3_TX_vect
#define DEBUG_RXINT USART3_RX_vect
#define DEBUG_PORT 	  UDR3
#define DEBUG_SPEED 115200
#define DEBUG_TXBUF 64
#define DEBUG_RXBUF 8
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
/*****************************************/

#if (DEBUG_LEVEL>=DEBUG_LEVEL_CRITICAL)
#define CRITICAL(msg) {debug(F("[CRITICAL]: ")); debug(msg); debug(F("!!!\r\n"));}
#define LEVEL_CRITICAL
#else
#define CRITICAL(msg)
#endif

#if (DEBUG_LEVEL>=DEBUG_LEVEL_WARNING)
#define WARNING(msg) {debug(F("[WARNING]: ")); debug(msg); debug(F("\r\n"));}
#define LEVEL_WARNING
#else
#define WARNING(msg)
#endif

#if (DEBUG_LEVEL>=DEBUG_LEVEL_INFO)
#define INFO(msg) {debug(F("[INFO]: ")); debug(msg); debug(F("\r\n"));}
#define LEVEL_INFO
#else
#define INFO(msg)
#endif

#if (DEBUG_LEVEL>=DEBUG_LEVEL_DEBUG)
#define DEBUG(msg) {debug(F("[DEBUG]: ")); debug(msg); debug(F("\r\n"));}
#define LEVEL_DEBUG
#else
#define DEBUG(msg) {debugPort(F("[DEBUG]: ")); debugPort(msg); debugPort(F("\r\n"));}
#endif

#if (DEBUG_LEVEL>=DEBUG_LEVEL_DATA)
#define DATA(msg) {debug(F("[DATA]: ")); debug(msg); debug(F("\r\n"));}
#define LEVEL_DATA
#else
#define DATA(msg)
#endif


#endif /* LOG_DEBUG_H_ */
