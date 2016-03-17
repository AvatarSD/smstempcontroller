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
				HWdata, _romBuff)
{
	init();

	HWdata.pinsSetup();
	startClock();
	_iface = &_sensorsIface;

	lcd_init (LCD_DISP_ON);
	lcd_led(0);

	debugSDcardLog.begin();
	debug(F("-------Hello-------\r\n"));

	_sensorsIface.DS2480B_Detect();
	debugSDcardLog.end();


}

void MainWorker::mainLoop()
{
	LED_ON;
	debugSDcardLog.begin();
//	debug(F("--------INT---------\r\n"));

	static int ntcRefrwshPeriodCount = 0;
	if (++ntcRefrwshPeriodCount == (NTC_REFRESH_PERIOD * 60 / TIMER_PERIOD))
	{
		_network.refreshTime();
		ntcRefrwshPeriodCount = 0;
	}

	_network.mainLoop();

	//debug(F("-------INTEND-------\r\n"));
	debugSDcardLog.end();
	LED_OFF;
}
/*
 //void MainWorker::screensaver(int time)
 //{
 //	char buff[50];
 //	sprintf(buff, "\nTime to next cycle:\n     %5d:%02d\n\n", time / 60,
 //			time % 60);
 //	debugLCD(buff);
 //}

 //void MainWorker::iterationTimerLoop()
 //{
 //	static bool mutex = false;
 //	static unsigned int breakCounter = 0;
 //
 //#ifdef LEVEL_DATA
 //	debug(F("[DATA]: ")); debug(F("breakCounter is: ")); debug(breakCounter);
 //	debug(F("   ")); debug(F("Mutex is: ")); debug(mutex); debug(F("\r\n"));
 //#endif
 //
 //	HWdata.checkPins();
 //
 //	if ((breakCounter == 0) && (!mutex))
 //	{
 //		mutex = true;
 //		wachdog.doCheckpoint();
 //		mainLoop();
 //		wachdog.doCheckpoint();
 //		mutex = false;
 //	}
 //	else if ((breakCounter != 0) && (!mutex))
 //		screensaver(TIMER_PERIOD * 60 - breakCounter);
 //
 //	++breakCounter;
 //	if ((breakCounter >= TIMER_PERIOD * 60) && (!mutex))
 //		breakCounter = 0;
 //}

 //void MainWorker::timerStart()
 //{
 //	// Timer/Counter 1 initialization
 //	// Clock source: System Clock
 //	// Clock value: 62,500 kHz
 //	// Mode: CTC top=ICR1
 //	// OC1A output: Disconnected
 //	// OC1B output: Disconnected
 //	// OC1C output: Disconnected
 //	// Noise Canceler: Off
 //	// Input Capture on Falling Edge
 //	// Timer Period: 1 s
 //	// Timer1 Overflow Interrupt: Off
 //	// Input Capture Interrupt: On
 //	// Compare A Match Interrupt: Off
 //	// Compare B Match Interrupt: Off
 //	// Compare C Match Interrupt: Off
 //	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0)
 //			| (0 << COM1C1) | (0 << COM1C0) | (0 << WGM11) | (0 << WGM10);
 //	TCCR1B = (0 << ICNC1) | (0 << ICES1) | (1 << WGM13) | (1 << WGM12)
 //			| (1 << CS12) | (0 << CS11) | (0 << CS10);
 //	TCNT1H = 0x00;
 //	TCNT1L = 0x00;
 //	ICR1H = 0xF4;
 //	ICR1L = 0x23;
 //
 //	wachdog.begin(WDT_SEC)	;
 //	//enable maim timer interrupt
 //	TIMSK1 |= (1 << ICIE1);
 //
 //}

 //void MainWorker::timerStop()
 //{
 //	TIMSK1 &= ~(0 << ICIE1);
 //	wachdog.end();
 //}*/

void MainWorker::startingProcedure()
{
//	char buff[40];
//
//	bool wasbtnNewSrcPressed = HWdata.isNewSrhBtnPress();
//	bool wasbtnAddSrcPressed = HWdata.isAddSrhBtnPress();
//
//	/*searching procedure*/
//	if ((wasbtnNewSrcPressed) || (wasbtnAddSrcPressed))
//	{
//		if (wasbtnNewSrcPressed)
//		{
//			/*cleaning data*/
//			memset(_mainbuf, 0, ROM_MAINBFF_SIZE * sizeof(ROM));
//			saveMainbuff();
//			INFO(F("Saved sensor ROMs was cleaned"));
//			INFO(F("New search perform"));
//		}
//		else
//			INFO(F("Search perform with only adding new sensors"));
//
//		uint16_t sensorsCount = 0;
//		while (true)
//		{
//			sensorsCount += _sensors.searchAllTempSensors(_mainbuf, ROM_MAINBFF_SIZE);
//
//#ifdef LEVEL_INFO
//			sprintf(buff, "New sensor founded: %d", sensorsCount);
//			INFO(buff);
//#endif
//
//			if ((HWdata.isNewSrhBtnPress()) || (HWdata.isAddSrhBtnPress()))
//			{
//				INFO(F("Perform to saving founding ROMs in eeprom memory"));
//				saveMainbuff();
//				INFO(F("Saving successful"));
//			}
//		}
//	}
//	else
//		timerStart();
}


