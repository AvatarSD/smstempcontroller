/*
 * rtc.h
 *
 *  Created on: 28 лист. 2015
 *      Author: sd
 */

#ifndef INIT_RTC_H_
#define INIT_RTC_H_

unsigned long long int getUnixTime();
void setUnixTime(unsigned long long int time);

void startClock();
void stopClock();

#endif /* INIT_RTC_H_ */
