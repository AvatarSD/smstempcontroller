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
	unsigned int tm_day;
	unsigned int tm_hour;
	unsigned int tm_min;
	unsigned int tm_sec;
};

// Internal RTC control
void startClock();
void stopClock();

// Internal RTC access
void setUnixTime(unsigned long long int time);
unsigned long long int getUnixTime();

// Additional functions
const char * getDateStr(const tm & time);
const char * getDateTimeStr(const tm & time);
const tm & convertFromUNIXtime(unsigned long long int time);
const unsigned long long int & convertToUNIXtime(const tm & time);

#endif /* INIT_RTC_H_ */
