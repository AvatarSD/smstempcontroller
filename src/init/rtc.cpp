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
char buff[BUFFER_SIZE] =
{ "DateTime" };

volatile unsigned long long int currentTime;

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
	TCCR3A=(0<<COM3A1) | (0<<COM3A0) | (0<<COM3B1) | (0<<COM3B0) | (0<<COM3C1) | (0<<COM3C0) | (0<<WGM31) | (0<<WGM30);
	TCCR3B=(0<<ICNC3) | (0<<ICES3) | (1<<WGM33) | (1<<WGM32) | (1<<CS32) | (0<<CS31) | (0<<CS30);

	ICR3=0xF423;

	// Timer/Counter 3 Interrupt(s) initialization
	TIMSK3=(1<<ICIE3) | (0<<OCIE3C) | (0<<OCIE3B) | (0<<OCIE3A) | (0<<TOIE3);

}

void stopClock()
{
	TIMSK3=(0<<ICIE3) | (0<<OCIE3C) | (0<<OCIE3B) | (0<<OCIE3A) | (0<<TOIE3);
}

const char* getDateStr()
{
	sprintf(buff,"-- %ld --", (unsigned long int)currentTime);
	return buff;
}

const char* getDateTimeStr()
{
	sprintf(buff,"Up date %ld sec", (unsigned long int)currentTime);
	return buff;
}

//
//struct Time
//{
//	unsigned int year;
//	unsigned int month;
//	unsigned int day;
//	unsigned int weekday;
//	unsigned int hour;
//	unsigned int minute;
//	unsigned int second;
//} time;
//
////unsigned long int Time_Convert_TO2TS(Time * time)
////{
////	if (time)
////	{
////		int mth[12] =
////		{ 0, 31, 59, 90, 120, 151, 181, 212, 273, 304, 334 };
////		int mthb[12] =
////		{ 0, 31, 60, 91, 121, 152, 182, 213, 274, 305, 335 };
////		Timestamp timestamp =
////				(((!(time->year % 4)) && (time->year % 100))
////						|| (!(time->year % 400))) ?
////						((((unsigned long int) (time->year - 1970) / 4))
////								+ (time->year - 1970) * 365
////								+ mthb[time->month - 1] + (time->day - 1))
////								* 86400 + time->hour * 3600 + time->minute * 60
////								+ time->second :
////						((((unsigned long int) (time->year - 1970) / 4))
////								+ (time->year - 1970) * 365
////								+ mth[time->month - 1] + (time->day - 1))
////								* 86400 + time->hour * 3600 + time->minute * 60
////								+ time->second;
////		return timestamp;
////	}
////	else
////	{
////		return 0;
////	}
////}
//
//Time * Time_Convert_TS2TO(unsigned long int _time)
//{
//	int i;
//	unsigned long int tmp;
//	Time * time = (Time *) malloc(sizeof(Time));
//	unsigned long int mth[12] =
//	{ 0, 31, 59, 90, 120, 151, 181, 212, 273, 304, 334, 365 };
//
//	time->second = _time % 60;
//	_time -= time->second;
//	time->minute = (_time % 3600) / 60;
//	_time -= (time->minute * 60);
//	time->hour = (unsigned int) (_time % ((unsigned long int) 24 * 3600))
//			/ 3600;
//	_time -= (unsigned long int) time->hour * 3600;
//	time->year = (unsigned int) (1970
//			+ _time / ((unsigned long int) 365 * 24 * 60 * 60));
//
//	for (i = 0, tmp = 1970; tmpyear;
//			(((!(tmp % 4)) && (tmp % 100)) || (!(tmp % 400))) ? i++ : 0, tmp++)
//		;
//	_time -= (unsigned long int) i * 24 * 60 * 60;
//	_time -= (unsigned long int) (time->year - 1970) * 365 * 24 * 60 * 60;
//	for (i = 1; i <= 12; i++)
//		if (mth[i] > (_time / ((unsigned long int) 24 * 60 * 60)))
//			break;
//	time->month = i;
//	_time -= mth[time->month - 1] * 24 * 60 * 60;
//	time->day = _time / ((unsigned long int) 24 * 60 * 60);
//
//	return time;
//}

