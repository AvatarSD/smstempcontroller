/*
 * uarts.cpp
 *
 * Created: 30.07.2015 15:17:53
 *  Author: sd
 */ 

#include "init.h"
#include <util/atomic.h>
#include "millis.h"
#include "rtc.h"


void init()
{
	// Crystal Oscillator division factor: 1
	CLKPR=(1<<CLKPCE);

	// Analog Comparator: Off
	ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);

	millisInit();
	startClock();
	// Global enable interrupts
	sei();

}
