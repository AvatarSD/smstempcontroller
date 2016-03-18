/*
 * MainWorker.cpp
 *
 *  Created on: 26 груд. 2015
 *      Author: sd
 */

#include "MainWorker.h"

#include "../LOG/debug.h"
#include "../init/rtc.h"
#include "../init/init.h"
#include "../init/wachdog/WachDog.h"
#include "../config.h"

#include <avr/interrupt.h>

#include <stdio.h>
#include <string>

UART * _iface;
ISR(DALL_RXINT)
{
	_iface->rx_byte_int();
}
ISR(DALL_TXINT)
{
	_iface->tx_byte_int();
}

MainWorker::MainWorker() :
		_sensorsIface(DALL_PORT), _sensors(_sensorsIface), _network(_sensors,
				HWdata)
{
	HWdata.pinsSetup();

	_iface = &_sensorsIface;

	lcd_init(LCD_DISP_ON);
	lcd_led(0);

	debugSDcardLog.begin();
	debug(F("-------Hello-------\r\n"));

	_sensorsIface.DS2480B_Detect();
	debugSDcardLog.end();

}

void MainWorker::mainLoop()
{
	debugSDcardLog.begin();
	LED_ON;

	static int lastDay = -1;
	int currDay = convertFromUNIXtime(getUnixTime()).tm_day;
//	if (lastDay != currDay)
//		_network.refreshTime();
//	lastDay = currDay;
//
//	_network.mainLoop();

	LED_OFF;
	debugSDcardLog.end();
}

//void MainWorker::startingProcedure()
//{
//
//}

