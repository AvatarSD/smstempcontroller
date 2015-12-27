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
#include "../LOG/SDCardLogger.h"
#include "../init/rtc.h"

#include <avr/eeprom.h>

ROM eepromMainbuf[ROM_MAINBFF_SIZE] EEMEM;

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
	init();
	HWdata.pinsSetup();
	startClock();
	_iface = &_sensorsIface;

	debugSDcardLog.begin();
	debug(F("-------Hello-------\r\n"));
	//_mainbuf = (ROM*)calloc(ROM_MAINBFF_SIZE, sizeof(ROM));
	lcd_init(LCD_DISP_ON);
	lcd_led(0);
	_sensorsIface.DS2480B_Detect();
	debugSDcardLog.end();
}

void MainWorker::mainLoop()
{
	LED_ON;
	debugSDcardLog.begin();
	debug(F("--------INT---------\r\n"));
	static int ntcRefrwshPeriodCount = 0;
	if (!ntcRefrwshPeriodCount)
		_network.refreshTime();
	if (++ntcRefrwshPeriodCount == (NTC_REFRESH_PERIOD * 60 / TIMER_PERIOD))
		ntcRefrwshPeriodCount = 0;
	while (!_network.sendTemp())
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

void MainWorker::timerStart()
{
	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: 62,500 kHz
	// Mode: CTC top=ICR1
	// OC1A output: Disconnected
	// OC1B output: Disconnected
	// OC1C output: Disconnected
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer Period: 1 s
	// Timer1 Overflow Interrupt: Off
	// Input Capture Interrupt: On
	// Compare A Match Interrupt: Off
	// Compare B Match Interrupt: Off
	// Compare C Match Interrupt: Off
	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0)
			| (0 << COM1C1) | (0 << COM1C0) | (0 << WGM11) | (0 << WGM10);
	TCCR1B = (0 << ICNC1) | (0 << ICES1) | (1 << WGM13) | (1 << WGM12)
			| (1 << CS12) | (0 << CS11) | (0 << CS10);
	TCNT1H = 0x00;
	TCNT1L = 0x00;
	ICR1H = 0xF4;
	ICR1L = 0x23;

	//enable maim timer interrupt
	TIMSK1 |= (1 << ICIE1);
}

void MainWorker::timerStop()
{
	TIMSK1 &= ~(0 << ICIE1);
}

void MainWorker::startingProcedure()
{
	bool wasbtnNewSrcPressed = HWdata.isNewSrhBtnPress();
	bool wasbtnAddSrcPressed = HWdata.isAddSrhBtnPress();

	loadMainbuff();

	/*searching procedure*/
	if ((wasbtnNewSrcPressed) || (wasbtnAddSrcPressed))
	{
		/*if wasbtnNewSrcPressed == true, cleaning data*/
		if (wasbtnNewSrcPressed)
		{
			memset(_mainbuf, 0, ROM_MAINBFF_SIZE * sizeof(ROM));
			saveMainbuff();
		}

		//todo: search algoritm
//

	}
	else
		timerStart();
}

void MainWorker::loadMainbuff()
{
	eeprom_read_block(_mainbuf, eepromMainbuf, ROM_MAINBFF_SIZE * sizeof(ROM));
}

void MainWorker::saveMainbuff()
{
	eeprom_write_block(_mainbuf, eepromMainbuf, ROM_MAINBFF_SIZE * sizeof(ROM));
}
