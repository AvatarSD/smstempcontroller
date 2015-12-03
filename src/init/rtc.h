/*
 * rtc.h
 *
 *  Created on: 28 лист. 2015
 *      Author: sd
 */

#ifndef INIT_RTC_H_
#define INIT_RTC_H_

struct tm
{
	unsigned int tm_year;
	unsigned int tm_mon;
	unsigned int tm_yday;
	unsigned int tm_mday;
	unsigned int tm_wday;
	unsigned int tm_hour;
	unsigned int tm_min;
	unsigned int tm_sec;
};

void startClock();
void stopClock();
void setUnixTime(unsigned long long int time);
unsigned long long int getUnixTime();
const char * getDateStr();
const char * getDateTimeStr();

const tm & gmtime(unsigned long long int time);

#endif /* INIT_RTC_H_ */
