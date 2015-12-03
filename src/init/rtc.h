/*
 * rtc.h
 *
 *  Created on: 28 лист. 2015
 *      Author: sd
 */

#ifndef INIT_RTC_H_
#define INIT_RTC_H_

void startClock();
void stopClock();
void setUnixTime(unsigned long long int time);
unsigned long long int getUnixTime();
const char * getDateStr();
const char * getDateTimeStr();

#endif /* INIT_RTC_H_ */
