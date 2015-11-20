/*
 * debug.cpp
 *
 *  Created on: 21 вер. 2015 р.
 *      Author: sd
 */

#include "stdlib.h"
#include "avr/interrupt.h"
#include "debug.h"


UART debugPort(DEBUG_PORT, DEBUG_SPEED, DEBUG_TXBUF, DEBUG_RXBUF);

ISR(DEBUG_RXINT)
{
	debugPort.rx_byte_int();
}
ISR(DEBUG_TXINT)
{
	debugPort.tx_byte_int();
}

