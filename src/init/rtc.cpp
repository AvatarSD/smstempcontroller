/*
 * rtc.cpp
 *
 *  Created on: 28 лист. 2015
 *      Author: sd
 */

#include "rtc.h"
#include "avr/io.h"
#include "avr/interrupt.h"
#include "stdio.h"

/** Settings **/
#define INT_VECTOR TIMER3_CAPT_vect
//#define TIM_ TIMER3_CAPT_vect
/**************/

#define BUFFER_SIZE 50
char buff[BUFFER_SIZE];
volatile unsigned long long int currentTime = 0;

ISR(INT_VECTOR)
{
	currentTime++;
}

unsigned long long int getUnixTime()
{
	return currentTime;
}

void setUnixTime(unsigned long long int time)
{
	currentTime = time;
}

void startClock()
{
	// Timer/Counter 3 initialization
	// Clock source: System Clock
	// Clock value: 62,500 kHz
	// Mode: CTC top=ICR3
	// OC3A output: Disconnected
	// OC3B output: Disconnected
	// OC3C output: Disconnected
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer Period: 1 s
	// Timer3 Overflow Interrupt: Off
	// Input Capture Interrupt: On
	// Compare A Match Interrupt: Off
	// Compare B Match Interrupt: Off
	// Compare C Match Interrupt: Off
	TCCR3A = (0 << COM3A1) | (0 << COM3A0) | (0 << COM3B1) | (0 << COM3B0)
			| (0 << COM3C1) | (0 << COM3C0) | (0 << WGM31) | (0 << WGM30);
	TCCR3B = (0 << ICNC3) | (0 << ICES3) | (1 << WGM33) | (1 << WGM32)
			| (1 << CS32) | (0 << CS31) | (0 << CS30);

	ICR3 = 0xF423;

	// Timer/Counter 3 Interrupt(s) initialization
	TIMSK3 = (1 << ICIE3) | (0 << OCIE3C) | (0 << OCIE3B) | (0 << OCIE3A)
			| (0 << TOIE3);

}

void stopClock()
{
	TIMSK3 = (0 << ICIE3) | (0 << OCIE3C) | (0 << OCIE3B) | (0 << OCIE3A)
			| (0 << TOIE3);
}

const char* getDateStr()
{
	const tm & time = gmtime(currentTime);
	sprintf(buff, "%02d-%02d-%04d", time.tm_mday, time.tm_mon, time.tm_year);
	return buff;
}

const char* getDateTimeStr()
{
	const tm & time = gmtime(currentTime);
	sprintf(buff, "%02d.%02d.%04d %02d:%02d:%02d", time.tm_mday, time.tm_mon, time.tm_year,
			time.tm_hour, time.tm_min, time.tm_sec);
	return buff;
}

/* Set the tm_t fields for the local time. */
const tm & gmtime(unsigned long long int time) //const time_t *timep;
{
	static tm tp;

	unsigned long long int day, mins, secs, year, leap;
	day = time / (24L * 60 * 60);
	secs = time % (24L * 60 * 60);
	tp.tm_sec = secs % 60;
	mins = secs / 60;
	tp.tm_hour = mins / 60;
	tp.tm_min = mins % 60;
	tp.tm_wday = (day + 4) % 7;
	year = (((day * 4) + 2) / 1461);
	tp.tm_year = year + 1970;
	leap = !(tp.tm_year & 3);
	day -= ((year * 1461) + 1) / 4;
	tp.tm_yday = day;
	day += (day > 58 + leap) ? ((leap) ? 1 : 2) : 0;
	tp.tm_mon = ((day * 12) + 6) / 367;
	tp.tm_mday = day + 1 - ((tp.tm_mon * 367) + 5) / 12;
	tp.tm_mon++;
	return tp;
}
