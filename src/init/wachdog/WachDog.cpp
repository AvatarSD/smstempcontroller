/*
 * wachdog.cpp
 *
 *  Created on: 11 січ. 2016
 *      Author: sd
 */

#include "WachDog.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

WachDog wachdog;

ISR(TIMER5_CAPT_vect)
{
	wachdog._intReset();
}

WachDog::WachDog() :
		_timeout(0), _curr(0)
{
}

void WachDog::begin()
{
	cli();

	// Timer/Counter 5 initialization
	// Clock source: System Clock
	// Clock value: 62,500 kHz
	// Mode: CTC top=ICR5
	// OC5A output: Disconnected
	// OC5B output: Disconnected
	// OC5C output: Disconnected
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer Period: 1 s
	// Timer5 Overflow Interrupt: Off
	// Input Capture Interrupt: On
	// Compare A Match Interrupt: Off
	// Compare B Match Interrupt: Off
	// Compare C Match Interrupt: Off
	TCCR5A = (0 << COM5A1) | (0 << COM5A0) | (0 << COM5B1) | (0 << COM5B0)
			| (0 << COM5C1) | (0 << COM5C0) | (0 << WGM51) | (0 << WGM50);
	TCCR5B = (0 << ICNC5) | (0 << ICES5) | (1 << WGM53) | (1 << WGM52)
			| (1 << CS52) | (0 << CS51) | (0 << CS50);
	TCNT5H = 0x00;
	TCNT5L = 0x00;
	ICR5H = 0xF4;
	ICR5L = 0x23;
	OCR5AH = 0x00;
	OCR5AL = 0x00;
	OCR5BH = 0x00;
	OCR5BL = 0x00;
	OCR5CH = 0x00;
	OCR5CL = 0x00;

	// Timer/Counter 5 Interrupt(s) initialization
	TIMSK5 = (1 << ICIE5) | (0 << OCIE5C) | (0 << OCIE5B) | (0 << OCIE5A)
			| (0 << TOIE5);

	wdt_reset();

	// Watchdog Timer initialization
	// Watchdog Timer Prescaler: OSC/1024k
	// Watchdog timeout action: Reset
	WDTCSR = (0 << WDIF) | (0 << WDIE) | (1 << WDP3) | (1 << WDCE) | (1 << WDE)
			| (0 << WDP2) | (0 << WDP1) | (1 << WDP0);
	WDTCSR = (0 << WDIF) | (0 << WDIE) | (1 << WDP3) | (0 << WDCE) | (1 << WDE)
			| (0 << WDP2) | (0 << WDP1) | (1 << WDP0);
	sei();
}

void WachDog::doCheckpoint()
{
	_curr = _timeout;
}

void WachDog::begin(uint16_t timeoutSec)
{
	setTimeout(timeoutSec);
	begin();
}

void WachDog::end()
{
	wdt_reset();
	TIMSK5 = 0;
	wdt_disable();
}

void WachDog::_intReset()
{
	_curr--;
	if (_curr > 0)
		wdt_reset();
}

void WachDog::setTimeout(uint16_t sec)
{
	_curr = sec;
	_timeout = sec;
}
