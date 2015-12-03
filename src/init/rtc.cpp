/*
 * rtc.cpp
 *
 *  Created on: 28 лист. 2015
 *      Author: sd
 */

#include "rtc.h"
#include "avr/io.h"
#include "avr/interrupt.h"

/** Settings **/
#define INT_VECTOR TIMER3_CAPT_vect
#define TIM_ TIMER3_CAPT_vect
/**************/

#define BUFFER_SIZE 50
char buff[BUFFER_SIZE] =
{ "Date:Time" };

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

}

void stopClock()
{

}

const char* getDateStr()
{
	return buff;
}

const char* getDateTimeStr()
{
	return buff;
}
