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
	if(true&&true/*todo hwdatacondition*/)
	{

	}
	else timerStart();
}
