/*
 * MainWorker.cpp
 *
 *  Created on: 26 груд. 2015
 *      Author: sd
 */

#include "MainWorker.h"

#include "avr/interrupt.h"
#include "stdio.h"
#include "../init/init.h"
#include "../LOG/debug.h"
#include "../Network/NetworkWorker.h"
#include "../config.h"
#include "../LOG/SDCardLogger.h"
#include "../init/rtc.h"



#define LED_ON //PORTB |= (1<<PORTB7)
#define LED_OFF //PORTB &=~ (1<<PORTB7)
#define LED_TRN //PORTB ^= (1<<PORTB7)

UART * _iface;
NetworkWorker * _network;

ISR(USART1_RX_vect)
{
	_iface->rx_byte_int();
}
ISR(USART1_TX_vect)
{
	_iface->tx_byte_int();
}

MainWorker::MainWorker()
{
	init();
	HWdata.pinsSetup();
	startClock();


	debugSDcardLog.begin();

	lcd_init(LCD_DISP_ON);
	lcd_led(0);




	DallasOneWire iface(UDR1);
	_iface = &iface;

	DallasTemp sensors(iface);

	NetworkWorker network(sensors, HWdata);
	_network = &network;

	debug(F("-------Hello-------\r\n"));

	iface.DS2480B_Detect();

	debugSDcardLog.end();
}

void MainWorker::mainLoop()
{
	LED_ON;
	debugSDcardLog.begin();
	debug(F("--------INT---------\r\n"));
	static int ntcRefrwshPeriodCount = 0;
	if (!ntcRefrwshPeriodCount)
		_network->refreshTime();
	if (++ntcRefrwshPeriodCount == (NTC_REFRESH_PERIOD * 60 / TIMER_PERIOD))
		ntcRefrwshPeriodCount = 0;
	while (!_network->sendTemp())
		;
	debug(F("-------INTEND-------\r\n"));
	debugSDcardLog.end();
	LED_OFF;
}

void MainWorker::screensaver(int time)
{
	char buff[50];
	sprintf(buff, "\nTime to next cycle:\n     %5d:%02d\n\n", time / 60,
			time % 60);
	debugLCD(buff);
}

void MainWorker::iterationTimerLoop()
{
	static bool mutex = false;
	static unsigned int breakCounter = 0;

#ifdef LEVEL_DATA
	debug(F("[DATA]: ")); debug(F("breakCounter is: ")); debug(breakCounter);
	debug(F("   ")); debug(F("Mutex is: ")); debug(mutex); debug(F("\r\n"));
#endif

	HWdata.checkPins();

	if ((breakCounter == 0) && (!mutex))
	{
		mutex = true;
		mainLoop();
		mutex = false;
	}
	else if ((breakCounter != 0) && (!mutex))
		screensaver(TIMER_PERIOD * 60 - breakCounter);

	++breakCounter;
	if ((breakCounter >= TIMER_PERIOD * 60) && (!mutex))
		breakCounter = 0;
}

