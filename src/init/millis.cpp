/*
 * millis.c
 *
 *  Created on: 6 вер. 2015 р.
 *      Author: sd
 */

#include "../init/millis.h"

#include "avr/io.h"
#include "avr/interrupt.h"

unsigned long long int ms;

void millisInit()
{
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 250,000 kHz
	// Mode: CTC top=OCR0A
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	// Timer Period: 1 ms
	TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (1<<WGM01) | (0<<WGM00);
	TCCR0B=(0<<WGM02) | (0<<CS02) | (1<<CS01) | (1<<CS00);
	TCNT0=0x00;
	OCR0A=0xF9;
	OCR0B=0x00;

	// Timer/Counter 0 Interrupt(s) initialization
	TIMSK0=(0<<OCIE0B) | (1<<OCIE0A) | (0<<TOIE0);

	ms = 0;
}

unsigned long long int millis()
{
	return ms;
}

ISR(TIMER0_COMPA_vect)
{
	ms++;
}
